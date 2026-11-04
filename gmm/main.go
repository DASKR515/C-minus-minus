package main

import (
	"archive/tar"
	"bufio"
	"context"
	"embed"
	"errors"
	"fmt"
	"io"
	"io/fs"
	"net/http"
	"os"
	"os/exec"
	"os/signal"
	"path/filepath"
	"regexp"
	"runtime"
	"strconv"
	"strings"
	"time"

	"github.com/ulikunitz/xz"
)

//go:embed libgmm/*
var libFS embed.FS

const (
	Author     = "DASKR"
	Repository = "https://github.com/DASKR515/C-minus-minus"
	Version    = "4.2"

	ghcWinURL   = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-unknown-mingw32-int_native.tar.xz"
	ghcLinuxURL = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-deb11-linux.tar.xz"
	// Upstream does not publish a deb11 aarch64 bindist; the alpine3_18 one
	// is the only aarch64 Linux tarball for this release.
	ghcLinuxARMURL = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-aarch64-alpine3_18-linux.tar.xz"
	ghcDarwinURL   = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-aarch64-apple-darwin.tar.xz"

	// Distribution specific bindists, picked by linuxGHCURL().
	ghcDarwinAMDURL   = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-apple-darwin.tar.xz"
	ghcLinuxAlpineURL = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-alpine3_12-linux.tar.xz"
	ghcLinuxDebURL    = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-deb12-linux.tar.xz"
	ghcLinuxRockyURL  = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-rocky8-linux.tar.xz"
	ghcLinuxFedoraURL = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-fedora38-linux.tar.xz"
	ghcLinuxUbuntuURL = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-x86_64-ubuntu20_04-linux.tar.xz"
	ghcLinuxARMDebURL = "https://downloads.haskell.org/~ghc/9.12.4/ghc-9.12.4-aarch64-deb10-linux.tar.xz"

	// llvmVersionRange is what GHC 9.12 accepts for the LLVM backend.
	llvmVersionRange = "13-20"

	confFileName = "conf.hmm"
	// DefaultTargetOutput keeps the historical "a.out" name for executables.
	DefaultTargetOutput = "a.out"
)

// Artifact records a file produced by the current run so that it can be
// cleaned without ever deleting something that existed before.
type Artifact struct {
	Path       string
	Preexisted bool
}

type Config struct {
	InputFile  string
	Output     string
	OutputSet  bool
	TargetType string // "exe", "linux", "so", "dll"
	Libs       []string
	ExtraObjs  []string
	CFiles     []string
	Includes   []string
	Defines    []string
	KeepObj    bool
	UseLLVM    bool
	UseRTS     bool
	CleanBuild bool
	RunAfter   bool
	EmitASM    bool
	Verbose    bool
	Strip      bool
	Timings    bool
	Watch      bool
	OptLevel   string // "", "0", "1", "2"
	GHCOpts    []string
	LDOpts     []string
	RunArgs    []string
}

func (c Config) IsShared() bool {
	return c.TargetType == "so" || c.TargetType == "dll"
}

type ToolPaths struct {
	GHC  string
	CC   string
	LLVM string
}

type ProgressWriter struct {
	Total       int64
	Downloaded  int64
	LastPrinted time.Time
	Label       string
	printed     bool
	frame       int
}

// spinnerFrames animates the symbol in front of the progress line.
var spinnerFrames = []string{"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"}

func (pw *ProgressWriter) Write(p []byte) (int, error) {
	n := len(p)
	pw.Downloaded += int64(n)

	if time.Since(pw.LastPrinted) > 100*time.Millisecond || (pw.Total > 0 && pw.Downloaded >= pw.Total) {
		pw.LastPrinted = time.Now()
		spin := spinnerFrames[pw.frame%len(spinnerFrames)]
		pw.frame++
		mbDownloaded := float64(pw.Downloaded) / (1024 * 1024)

		if pw.Total > 0 {
			mbTotal := float64(pw.Total) / (1024 * 1024)
			percent := float64(pw.Downloaded) / float64(pw.Total) * 100
			if percent > 100 {
				percent = 100
			}
			barLen := 30
			completed := int((percent / 100) * float64(barLen))
			if completed > barLen {
				completed = barLen
			}
			if completed < 0 {
				completed = 0
			}
			bar := strings.Repeat("█", completed) + strings.Repeat("░", barLen-completed)

			fmt.Printf("\r %s %s [%s] %.1f%% (%.2f / %.2f MB)", spin, pw.Label, bar, percent, mbDownloaded, mbTotal)
		} else {
			fmt.Printf("\r %s %s (%.2f MB downloaded)", spin, pw.Label, mbDownloaded)
		}
		pw.printed = true
	}
	return n, nil
}

// Finish closes the progress line so later output starts on a fresh row.
func (pw *ProgressWriter) Finish() {
	if !pw.printed {
		return
	}
	pw.printed = false
	if pw.Total > 0 {
		fmt.Printf("\r ◆ %s [%.2f MB] done\n", pw.Label, float64(pw.Total)/(1024*1024))
		return
	}
	fmt.Printf("\r ◆ %s (%.2f MB) done\n", pw.Label, float64(pw.Downloaded)/(1024*1024))
}

// execMode is set once from the command line and consulted by every helper
// that starts an external process, so -verbose and -n behave the same
// everywhere without threading the whole Config through each call.
var execMode struct {
	Verbose bool
	DryRun  bool
}

// errReported marks a failure whose details were already shown to the user
// (GHC's own diagnostics, for instance), so main does not print a second line.
var errReported = errors.New("build failed")

func quoteCommand(name string, args []string) string {
	parts := make([]string, 0, len(args)+1)
	for _, p := range append([]string{name}, args...) {
		if p == "" || strings.ContainsAny(p, " \t\"'") {
			p = strconv.Quote(p)
		}
		parts = append(parts, p)
	}
	return strings.Join(parts, " ")
}

// echoCommand shows a command before it runs (-verbose) or instead of running
// it (-n). It returns true when the caller must skip the real execution.
func echoCommand(name string, args []string) bool {
	if execMode.DryRun {
		fmt.Println("[dry-run]", quoteCommand(name, args))
		return true
	}
	if execMode.Verbose {
		fmt.Fprintln(os.Stderr, "+", quoteCommand(name, args))
	}
	return false
}

// ensureDir creates dir unless it is the current directory or -n is active.
func ensureDir(dir string) error {
	if dir == "" || dir == "." || execMode.DryRun {
		return nil
	}
	return os.MkdirAll(dir, 0755)
}

// isExitError reports whether err means the tool ran and returned a failure
// status (its own diagnostics are already on screen) rather than failing to start.
func isExitError(err error) bool {
	var exitErr *exec.ExitError
	return errors.As(err, &exitErr)
}

// toolFailure keeps the short message for a tool that ran and failed, and adds
// the cause when the tool could not even be started.
func toolFailure(what string, err error) error {
	if isExitError(err) {
		return errors.New(what)
	}
	return fmt.Errorf("%s: %v", what, err)
}

// Stopwatch records how long each build phase took (-time).
type Stopwatch struct {
	Enabled bool
	started time.Time
	mark    time.Time
	laps    []stopwatchLap
}

type stopwatchLap struct {
	Name string
	Took time.Duration
}

func newStopwatch(enabled bool) *Stopwatch {
	now := time.Now()
	return &Stopwatch{Enabled: enabled, started: now, mark: now}
}

// Lap closes the current phase and starts the next one.
func (s *Stopwatch) Lap(name string) {
	if !s.Enabled {
		return
	}
	now := time.Now()
	s.laps = append(s.laps, stopwatchLap{Name: name, Took: now.Sub(s.mark)})
	s.mark = now
}

func (s *Stopwatch) Report() {
	if !s.Enabled || len(s.laps) == 0 {
		return
	}
	fmt.Println("Timings:")
	for _, l := range s.laps {
		fmt.Printf("  %-10s %s\n", l.Name, l.Took.Round(time.Millisecond))
	}
	fmt.Printf("  %-10s %s\n", "total", time.Since(s.started).Round(time.Millisecond))
}

func printHelp() {
	helpText := fmt.Sprintf(`GMM (GHC C-- Manager) v%s - Build tool and compiler driver for C-- projects.
Developed by: %s
Repository:   %s

USAGE:
  gmm [flags] <input.cmm> [extra_files.o/a / dynamic_libs.so...]
  gmm [flags] build | -b
  gmm [flags] bc | -bc
  gmm [flags] run | -r [executable]
  gmm [flags] br | -br
  gmm [flags] watch | -w
  gmm [flags] clean | -clean
  gmm [flags] init | -init
  gmm doctor | -doctor

FLAGS:
  -o <file>        Specify output binary name or target format (e.g. -o app or -o C="dll").
  -o C="<target>"  Specify the output format. Each name matches the file produced:
                     native  plain executable for this host (default, no format)
                     exe     Windows PE executable (.exe)
                     dll     Windows PE shared library (.dll)
                     linux   Linux ELF executable (.elf)
                     so      native shared object (.so)
                   Example: gmm -o C="linux" -o app main.cmm
                   Both forms can be combined: gmm -o C="so" -o libapp main.cmm
                   A target for another OS is refused; gmm does not cross-compile.
  -I <path>        Add include directory for header files (.h). Can be used multiple times.
  -D <name[=val]>  Define a macro for the C-- preprocessor. Can be used multiple times.
  -S               Emit assembly code (.s) instead of building an executable/library.
  -b               Build project using configuration from 'conf.hmm'.
  -bc              Build project and clean ONLY object files dynamically created by gmm in this run.
  -r, -run         Run the output executable.
  -br              Build project using conf.hmm and execute the binary immediately upon success.
  -libs <libs>     Comma-separated list of additional system libraries or paths to link with.
                   Can be used multiple times.
  -keep-obj        Keep intermediate object files (.o) after linking instead of deleting them.
  -llvm            Use GHC LLVM backend (-fllvm) during C-- compilation.
  -rts             Use GHC directly as the linker driver instead of system C compiler (CC).
  -opt <0|1|2>     Optimization level handed to GHC (-O0, -O1, -O2).
  -ghc-opt <f>     Comma-separated extra flags passed straight to GHC. Can be used multiple times.
  -ld-opt <f>      Comma-separated extra flags passed to the linker. Can be used multiple times.
  -strip           Strip symbols from the linked output.
  -arg <value>     Argument given to the program started by -r / -br. Can be used multiple times.
  -w, -watch       Rebuild whenever the source, a header or conf.hmm changes (Ctrl-C to stop).
                   Combine with -r to run the program after every successful build.
  -clean           Remove the files gmm produces for this project (objects and the output).
  -init            Write a conf.hmm from the current command line.
  -f, -force       Let -init overwrite an existing conf.hmm.
  -n, -dry-run     Print the commands that would run without running anything.
  -time            Print how long each build phase took.
  -verbose         Echo every external command before it runs.
  -doctor          Check the toolchain and the environment, then exit.
  -v, -version     Display version and developer information.
  -h, -help        Display this help message and exit.

CONF.HMM KEYS:
  input=<file.cmm>          output=<name>            target=<native|exe|dll|linux|so>
  includes=<d1,d2>          defines=<D1,D2>          cfiles=<a.c,b.c>
  libs=<m,pthread>          keepobj=<true|false>     emitasm=<true|false>
  llvm=<true|false>         rts=<true|false>         run=<true|false>
  opt=<0|1|2>               strip=<true|false>       args=<a1,a2>
  ghcopts=<f1,f2>           ldopts=<f1,f2>           objs=<a.o,b.o>

ENVIRONMENT:
  GMM_HOME      Installation directory that holds 'Ghc/' and 'sit.txt'.
  GMM_GHC       Path to a ghc binary (skips the bundled toolchain lookup).
  GMM_CC        Path to the C compiler driver used for assembling/linking.
  GMM_LLVM      Path to the LLVM 'opt' tool.
`, Version, Author, Repository)

	fmt.Print(helpText)
}

func printVersion() {
	fmt.Printf("gmm version %s\nDeveloper: %s\nGitHub: %s\n", Version, Author, Repository)
}

// ---------------------------------------------------------------------------
// Installation directory / toolchain discovery
// ---------------------------------------------------------------------------

func installDirCandidates() []string {
	var dirs []string
	if env := strings.TrimSpace(os.Getenv("GMM_HOME")); env != "" {
		return []string{env}
	}
	if runtime.GOOS == "windows" {
		pf := strings.TrimSpace(os.Getenv("ProgramFiles(x86)"))
		if pf == "" {
			pf = strings.TrimSpace(os.Getenv("ProgramFiles"))
		}
		if pf == "" {
			pf = `C:\Program Files (x86)`
		}
		return []string{filepath.Join(pf, "gmm")}
	}
	dirs = append(dirs, "/usr/local/bin/gmm")
	if home, err := os.UserHomeDir(); err == nil && home != "" {
		dirs = append(dirs, filepath.Join(home, ".local", "share", "gmm"))
	}
	return dirs
}

func sitFilePathIn(dir string) string {
	return filepath.Join(dir, "sit.txt")
}

func loadToolPathsFrom(dir string) (ToolPaths, error) {
	sitPath := sitFilePathIn(dir)
	file, err := os.Open(sitPath)
	if err != nil {
		return ToolPaths{}, err
	}
	defer file.Close()

	tp := ToolPaths{}
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}
		parts := strings.SplitN(line, "=", 2)
		if len(parts) != 2 {
			continue
		}
		key := strings.ToLower(strings.TrimSpace(parts[0]))
		val := strings.TrimSpace(parts[1])
		switch key {
		case "ghc":
			tp.GHC = val
		case "cc":
			tp.CC = val
		case "llvm":
			tp.LLVM = val
		}
	}
	if err := scanner.Err(); err != nil {
		return ToolPaths{}, err
	}
	if tp.GHC == "" {
		return ToolPaths{}, fmt.Errorf("%s does not define a GHC path", sitPath)
	}
	return tp, nil
}

func saveSitFileIn(dir string, tp ToolPaths) error {
	if err := os.MkdirAll(dir, 0755); err != nil {
		return err
	}
	content := fmt.Sprintf("GHC=%s\nCC=%s\nLLVM=%s\n", tp.GHC, tp.CC, tp.LLVM)
	return os.WriteFile(sitFilePathIn(dir), []byte(content), 0644)
}

func isExecutableFile(path string) bool {
	info, err := os.Stat(path)
	if err != nil || info.IsDir() {
		return false
	}
	if runtime.GOOS == "windows" {
		return true
	}
	return info.Mode().Perm()&0111 != 0
}

func toolRole(name string) string {
	n := strings.ToLower(name)
	if n == "ghc.exe" {
		n = "ghc"
	}
	switch n {
	case "ghc":
		return "ghc"
	case "gcc", "gcc.exe":
		return "cc-gcc"
	case "clang", "clang.exe":
		return "cc-clang"
	case "cc", "cc.exe":
		return "cc-cc"
	case "opt", "opt.exe":
		return "llvm"
	}
	if strings.HasPrefix(n, "ghc-") && isVersionedName(strings.TrimPrefix(n, "ghc-")) {
		return "ghc-versioned"
	}
	return ""
}

func isVersionedName(s string) bool {
	if s == "" {
		return false
	}
	return s[0] >= '0' && s[0] <= '9'
}

// toolScore ranks the candidates found while walking a toolchain directory so
// the real driver always wins over helper binaries that live next to it.
func toolScore(role, path string, info os.FileInfo) int {
	base := 0
	switch role {
	case "ghc":
		base = 100
	case "ghc-versioned":
		base = 80
	case "cc-gcc":
		base = 90
	case "cc-clang":
		base = 85
	case "cc-cc":
		base = 70
	case "llvm":
		base = 90
	default:
		return -1
	}
	dir := filepath.ToSlash(filepath.Dir(path))
	if strings.HasSuffix(dir, "/bin") || strings.Contains(dir, "/bin/") {
		base += 10
	}
	if runtime.GOOS != "windows" && info.Mode().Perm()&0111 != 0 {
		base += 5
	}
	return base
}

func findToolsInDir(dir string) (ToolPaths, bool) {
	var tp ToolPaths
	if info, err := os.Stat(dir); err != nil || !info.IsDir() {
		return tp, false
	}

	scores := map[string]int{}
	assign := func(role, path string, info os.FileInfo) {
		score := toolScore(role, path, info)
		if score < 0 {
			return
		}
		if prev, ok := scores[role]; ok && prev >= score {
			return
		}
		scores[role] = score
		switch role {
		case "ghc", "ghc-versioned":
			tp.GHC = path
		case "cc-gcc", "cc-clang", "cc-cc":
			tp.CC = path
		case "llvm":
			tp.LLVM = path
		}
	}

	_ = filepath.Walk(dir, func(path string, info os.FileInfo, err error) error {
		if err != nil || info == nil {
			return nil
		}
		if info.Mode()&os.ModeSymlink != 0 {
			resolved, statErr := os.Stat(path)
			if statErr != nil || resolved.IsDir() {
				return nil
			}
			info = resolved
		}
		if info.IsDir() || !info.Mode().IsRegular() {
			return nil
		}
		if strings.HasSuffix(strings.ToLower(info.Name()), ".tar.xz") {
			return nil
		}
		assign(toolRole(info.Name()), path, info)
		return nil
	})

	if tp.GHC == "" && tp.CC != "" {
		// A C compiler alone is not a usable toolchain for C--.
		return ToolPaths{}, false
	}
	if tp.CC == "" && tp.GHC != "" {
		tp.CC = guessCC(tp.GHC)
	}
	if tp.LLVM == "" {
		tp.LLVM = lookupInPath("opt", "opt.exe")
	}

	return tp, tp.GHC != "" && isExecutableFile(tp.GHC)
}

func lookupInPath(names ...string) string {
	for _, name := range names {
		if p, err := exec.LookPath(name); err == nil && p != "" {
			return p
		}
	}
	return ""
}

func guessCC(ghcPath string) string {
	dir := filepath.Dir(ghcPath)
	if runtime.GOOS == "windows" {
		for _, name := range []string{"clang.exe", "gcc.exe", "cc.exe"} {
			candidate := filepath.Join(dir, name)
			if isExecutableFile(candidate) {
				return candidate
			}
		}
	}
	if p := lookupInPath("gcc", "clang", "cc"); p != "" {
		return p
	}
	candidate := filepath.Join(dir, "gcc")
	if isExecutableFile(candidate) {
		return candidate
	}
	return ""
}

func envToolPaths() ToolPaths {
	tp := ToolPaths{
		GHC:  strings.TrimSpace(os.Getenv("GMM_GHC")),
		CC:   strings.TrimSpace(os.Getenv("GMM_CC")),
		LLVM: strings.TrimSpace(os.Getenv("GMM_LLVM")),
	}
	if tp.GHC == "" {
		return ToolPaths{}
	}
	if !isExecutableFile(tp.GHC) {
		// An explicit override that does not exist is a user error; silently
		// falling back to the bundled toolchain hides the mistake.
		fmt.Fprintf(os.Stderr, "error: GMM_GHC=%s is not an executable file\n", tp.GHC)
		fmt.Fprintln(os.Stderr, "unset GMM_GHC to use the bundled toolchain")
		os.Exit(1)
	}
	if tp.CC == "" {
		tp.CC = guessCC(tp.GHC)
	}
	if tp.LLVM == "" {
		tp.LLVM = lookupInPath("opt", "opt.exe")
	}
	return tp
}

func validToolPaths(tp ToolPaths) bool {
	if tp.GHC == "" || !isExecutableFile(tp.GHC) {
		return false
	}
	if tp.CC != "" && !isExecutableFile(tp.CC) {
		return false
	}
	if tp.LLVM != "" && !isExecutableFile(tp.LLVM) {
		return false
	}
	return true
}

func dirIsWritable(dir string) bool {
	if err := os.MkdirAll(dir, 0755); err != nil {
		return false
	}
	probe, err := os.CreateTemp(dir, ".gmm-probe-*")
	if err != nil {
		return false
	}
	name := probe.Name()
	probe.Close()
	os.Remove(name)
	return true
}

func setupEnvironment() (ToolPaths, error) {
	if tp := envToolPaths(); tp.GHC != "" {
		return tp, nil
	}

	// A GHC installed by the system package manager always comes first.
	if tp, ok := systemGHCToolPaths(); ok {
		if execMode.Verbose {
			fmt.Println("Using system GHC", tp.GHC)
		}
		return tp, nil
	}

	candidates := installDirCandidates()

	for _, dir := range candidates {
		tp, err := loadToolPathsFrom(dir)
		if err != nil {
			continue
		}
		if !validToolPaths(tp) {
			continue
		}
		if tp.CC == "" {
			tp.CC = guessCC(tp.GHC)
		}
		if tp.LLVM == "" {
			tp.LLVM = lookupInPath("opt", "opt.exe")
		}
		return tp, nil
	}

	for _, dir := range candidates {
		ghcDir := filepath.Join(dir, "Ghc")
		if tp, ok := findToolsInDir(ghcDir); ok {
			fmt.Println("Found existing GHC environment in", ghcDir)
			if err := saveSitFileIn(dir, tp); err != nil {
				fmt.Fprintf(os.Stderr, "warning: could not write %s: %v\n", sitFilePathIn(dir), err)
			}
			return tp, nil
		}
	}

	if execMode.DryRun {
		return ToolPaths{}, errors.New("the GHC toolchain is not installed and -n will not download it (run gmm once without -n)")
	}

	baseDir := ""
	for _, dir := range candidates {
		if dirIsWritable(dir) {
			baseDir = dir
			break
		}
	}
	if baseDir == "" {
		return ToolPaths{}, fmt.Errorf("no writable installation directory found (tried %s); set GMM_HOME to a writable path",
			strings.Join(candidates, ", "))
	}

	ghcExtractDir := filepath.Join(baseDir, "Ghc")
	fmt.Printf("GHC environment not detected in %s. Starting setup...\n", baseDir)

	downloadURL, err := ghcDownloadURL()
	if err != nil {
		return ToolPaths{}, err
	}
	tarPath := filepath.Join(baseDir, "ghc.tar.xz")
	tmpTarPath := tarPath + ".tmp"

	if info, err := os.Stat(tarPath); err != nil || info.Size() == 0 {
		if err := os.Remove(tmpTarPath); err != nil && !os.IsNotExist(err) {
			return ToolPaths{}, err
		}
		fmt.Println("Downloading GHC toolchain...")
		if err := downloadFile(downloadURL, tmpTarPath); err != nil {
			os.Remove(tmpTarPath)
			return ToolPaths{}, fmt.Errorf("download error: %w", err)
		}
		if err := os.Rename(tmpTarPath, tarPath); err != nil {
			return ToolPaths{}, fmt.Errorf("file rename error: %w", err)
		}
	} else {
		fmt.Println("Found existing archive, skipping download...")
	}

	fmt.Println("\n◇ Extracting GHC toolchain (this may take a minute)...")
	if err := extractTarXz(tarPath, ghcExtractDir); err != nil {
		os.Remove(tarPath)
		return ToolPaths{}, fmt.Errorf("extraction error: %w", err)
	}
	fmt.Println("\n◈ Extraction complete.")
	os.Remove(tarPath)

	extractedTp, ok := findToolsInDir(ghcExtractDir)
	if !ok {
		return ToolPaths{}, errors.New("could not locate GHC binaries after extraction")
	}
	if extractedTp.CC == "" {
		extractedTp.CC = guessCC(extractedTp.GHC)
	}
	if extractedTp.LLVM == "" {
		extractedTp.LLVM = lookupInPath("opt", "opt.exe")
	}

	if err := saveSitFileIn(baseDir, extractedTp); err != nil {
		fmt.Fprintf(os.Stderr, "warning: could not write %s: %v\n", sitFilePathIn(baseDir), err)
	}
	fmt.Println("GHC environment successfully configured.")
	return extractedTp, nil
}

func ghcDownloadURL() (string, error) {
	switch runtime.GOOS {
	case "windows":
		if runtime.GOARCH == "arm64" {
			return "", errors.New("no GHC bindist is published for windows/arm64; " +
				"install GHC yourself and point GMM_GHC at it")
		}
		return ghcWinURL, nil
	case "darwin":
		if runtime.GOARCH == "arm64" {
			return ghcDarwinURL, nil
		}
		if runtime.GOARCH == "amd64" {
			return ghcDarwinAMDURL, nil
		}
		return "", errors.New("no GHC bindist is published for darwin/" + runtime.GOARCH + "; " +
			"install GHC yourself and point GMM_GHC at it")
	default:
		return linuxGHCURL(), nil
	}
}

// readOSRelease parses /etc/os-release (or /usr/lib/os-release) into a map.
func readOSRelease() map[string]string {
	data, err := os.ReadFile("/etc/os-release")
	if err != nil {
		data, err = os.ReadFile("/usr/lib/os-release")
		if err != nil {
			return nil
		}
	}
	out := map[string]string{}
	for _, line := range strings.Split(string(data), "\n") {
		line = strings.TrimSpace(line)
		if line == "" || strings.HasPrefix(line, "#") {
			continue
		}
		parts := strings.SplitN(line, "=", 2)
		if len(parts) != 2 {
			continue
		}
		out[strings.ToUpper(strings.TrimSpace(parts[0]))] = trimQuotes(parts[1])
	}
	return out
}

// detectLinuxFamily maps the running distribution (ID / ID_LIKE) to the family
// of the GHC bindist built for it: alpine, ubuntu, debian, rhel or fedora.
// An empty result means the distribution is unknown.
func detectLinuxFamily() string {
	osr := readOSRelease()
	ids := strings.Fields(strings.ToLower(osr["ID"] + " " + osr["ID_LIKE"]))
	has := func(names ...string) bool {
		for _, id := range ids {
			for _, n := range names {
				if id == n {
					return true
				}
			}
		}
		return false
	}
	switch {
	case has("alpine"):
		return "alpine"
	case has("ubuntu"):
		return "ubuntu"
	case has("debian"):
		return "debian"
	// RHEL clones list "fedora" in ID_LIKE, so rhel must be tested first.
	case has("rhel", "centos", "rocky", "almalinux"):
		return "rhel"
	case has("fedora"):
		return "fedora"
	}
	return ""
}

// linuxGHCURL picks the GHC bindist that matches this Linux distribution.
func linuxGHCURL() string {
	family := detectLinuxFamily()

	if runtime.GOARCH == "arm64" {
		switch family {
		case "debian", "ubuntu", "rhel", "fedora":
			return ghcLinuxARMDebURL
		}
		return ghcLinuxARMURL
	}

	switch family {
	case "alpine":
		return ghcLinuxAlpineURL
	case "ubuntu":
		return ghcLinuxUbuntuURL
	case "debian":
		return ghcLinuxDebURL
	case "rhel":
		return ghcLinuxRockyURL
	case "fedora":
		return ghcLinuxFedoraURL
	}
	return ghcLinuxURL
}

// systemGHCToolPaths returns a GHC that is already installed on the system
// (found in PATH, e.g. from apt/dnf/apk/pacman). A GHC that lives inside gmm's
// own installation directory is ignored so the bundled toolchain keeps working
// the way it did before.
func systemGHCToolPaths() (ToolPaths, bool) {
	if runtime.GOOS != "linux" {
		return ToolPaths{}, false
	}
	ghc := lookupInPath("ghc")
	if ghc == "" || !isExecutableFile(ghc) {
		return ToolPaths{}, false
	}
	resolved := ghc
	if r, err := filepath.EvalSymlinks(ghc); err == nil {
		resolved = r
	}
	for _, dir := range installDirCandidates() {
		prefix := filepath.Clean(dir) + string(os.PathSeparator)
		if strings.HasPrefix(resolved, prefix) || strings.HasPrefix(ghc, prefix) {
			return ToolPaths{}, false
		}
	}
	if toolVersionLine(ghc, "--numeric-version") == "" {
		return ToolPaths{}, false
	}
	return ToolPaths{
		GHC:  ghc,
		CC:   guessCC(ghc),
		LLVM: lookupInPath("opt", "opt.exe"),
	}, true
}

func downloadFile(url string, filepathStr string) error {
	ctx, cancel := context.WithTimeout(context.Background(), 3*time.Hour)
	defer cancel()

	req, err := http.NewRequestWithContext(ctx, "GET", url, nil)
	if err != nil {
		return err
	}
	req.Header.Set("User-Agent", fmt.Sprintf("gmm/%s (+%s)", Version, Repository))

	client := &http.Client{}
	resp, err := client.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		return fmt.Errorf("bad HTTP status: %s", resp.Status)
	}

	contentLength, _ := strconv.ParseInt(resp.Header.Get("Content-Length"), 10, 64)

	out, err := os.Create(filepathStr)
	if err != nil {
		return err
	}
	defer out.Close()

	pw := &ProgressWriter{
		Total: contentLength,
		Label: "Downloading GHC",
	}
	defer pw.Finish()

	buf := make([]byte, 256*1024)
	written, err := io.CopyBuffer(out, io.TeeReader(resp.Body, pw), buf)
	if err != nil {
		return err
	}
	if contentLength > 0 && written != contentLength {
		return fmt.Errorf("incomplete download: got %d bytes, want %d", written, contentLength)
	}
	if written == 0 {
		return errors.New("downloaded file is empty")
	}
	return out.Close()
}

// safeJoin keeps extracted archives from escaping the destination directory.
func safeJoin(destDir, name string) (string, error) {
	cleaned := filepath.Clean(filepath.FromSlash(name))
	if filepath.IsAbs(cleaned) || cleaned == ".." || strings.HasPrefix(cleaned, ".."+string(os.PathSeparator)) {
		return "", fmt.Errorf("illegal archive entry %q", name)
	}
	target := filepath.Join(destDir, cleaned)
	rel, err := filepath.Rel(destDir, target)
	if err != nil || rel == ".." || strings.HasPrefix(rel, ".."+string(os.PathSeparator)) {
		return "", fmt.Errorf("illegal archive entry %q", name)
	}
	return target, nil
}

func extractTarXz(tarXzPath, destDir string) error {
	f, err := os.Open(tarXzPath)
	if err != nil {
		return err
	}
	defer f.Close()

	fileInfo, err := f.Stat()
	if err != nil {
		return err
	}

	pw := &ProgressWriter{
		Total: fileInfo.Size(),
		Label: "Decompressing XZ",
	}
	defer pw.Finish()

	bufferedReader := bufio.NewReaderSize(io.TeeReader(f, pw), 1024*1024)
	xzReader, err := xz.NewReader(bufferedReader)
	if err != nil {
		return err
	}

	tarReader := tar.NewReader(xzReader)
	buf := make([]byte, 1024*1024)

	for {
		header, err := tarReader.Next()
		if err == io.EOF {
			break
		}
		if err != nil {
			return err
		}

		target, err := safeJoin(destDir, header.Name)
		if err != nil {
			return err
		}

		switch header.Typeflag {
		case tar.TypeDir:
			if err := os.MkdirAll(target, 0755); err != nil {
				return err
			}
		case tar.TypeReg:
			if err := os.MkdirAll(filepath.Dir(target), 0755); err != nil {
				return err
			}
			mode := os.FileMode(header.Mode).Perm()
			if mode == 0 {
				mode = 0644
			}
			outFile, err := os.OpenFile(target, os.O_CREATE|os.O_RDWR|os.O_TRUNC, mode)
			if err != nil {
				return err
			}
			if _, err := io.CopyBuffer(outFile, tarReader, buf); err != nil {
				outFile.Close()
				return err
			}
			if err := outFile.Close(); err != nil {
				return err
			}
			if err := os.Chmod(target, mode); err != nil && runtime.GOOS != "windows" {
				return err
			}
		case tar.TypeSymlink:
			linkTarget, err := safeJoin(filepath.Dir(target), header.Linkname)
			if err != nil {
				// Skip links that point outside of the tree instead of aborting.
				continue
			}
			if err := os.MkdirAll(filepath.Dir(target), 0755); err != nil {
				return err
			}
			os.Remove(target)
			if err := os.Symlink(linkTarget, target); err != nil && !errors.Is(err, os.ErrExist) {
				return err
			}
		case tar.TypeLink:
			linkSource, err := safeJoin(destDir, header.Linkname)
			if err != nil {
				continue
			}
			if err := os.MkdirAll(filepath.Dir(target), 0755); err != nil {
				return err
			}
			os.Remove(target)
			if err := os.Link(linkSource, target); err != nil && !errors.Is(err, os.ErrExist) {
				return err
			}
		default:
			// Character/block devices, fifos and pax headers are not needed.
			continue
		}
	}
	return nil
}

// ---------------------------------------------------------------------------
// Command line parsing
// ---------------------------------------------------------------------------

type arrayFlags []string

func (i *arrayFlags) String() string {
	return strings.Join(*i, ",")
}

func (i *arrayFlags) Set(value string) error {
	for _, part := range splitList(value) {
		*i = append(*i, part)
	}
	return nil
}

type cliFlags struct {
	outputRaw   string
	outputSeen  bool
	targetTypes arrayFlags
	includes    arrayFlags
	defines     arrayFlags
	libs        arrayFlags
	emitASM     bool
	keepObj     bool
	version     bool
	useLLVM     bool
	useRTS      bool
	verbose     bool
	buildCmd    bool
	buildClean  bool
	runCmd      bool
	buildRun    bool
	help        bool
	optLevel    string
	ghcOpts     arrayFlags
	ldOpts      arrayFlags
	runArgs     []string
	strip       bool
	timings     bool
	dryRun      bool
	watch       bool
	clean       bool
	initCmd     bool
	force       bool
	doctor      bool

	positional []string
}

type flagSpec struct {
	names    []string
	boolean  bool
	assigned func(c *cliFlags, value string) error
}

func flagSpecs() []flagSpec {
	return []flagSpec{
		{names: []string{"o"}, assigned: func(c *cliFlags, v string) error {
			if strings.HasPrefix(v, "C=") {
				return c.targetTypes.Set(strings.TrimPrefix(v, "C="))
			}
			c.outputRaw = v
			c.outputSeen = true
			return nil
		}},
		{names: []string{"i"}, assigned: func(c *cliFlags, v string) error {
			return c.includes.Set(v)
		}},
		{names: []string{"d", "define"}, assigned: func(c *cliFlags, v string) error {
			return c.defines.Set(v)
		}},
		{names: []string{"libs", "lib"}, assigned: func(c *cliFlags, v string) error {
			return c.libs.Set(v)
		}},
		{names: []string{"s"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.emitASM = b
			return err
		}},
		{names: []string{"keep-obj", "keepobj"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.keepObj = b
			return err
		}},
		{names: []string{"llvm"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.useLLVM = b
			return err
		}},
		{names: []string{"rts"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.useRTS = b
			return err
		}},
		{names: []string{"v", "version"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.version = b
			return err
		}},
		{names: []string{"verbose"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.verbose = b
			return err
		}},
		{names: []string{"b", "build"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.buildCmd = b
			return err
		}},
		{names: []string{"bc"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.buildClean = b
			return err
		}},
		{names: []string{"r", "run"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.runCmd = b
			return err
		}},
		{names: []string{"br"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.buildRun = b
			return err
		}},
		{names: []string{"h", "help"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.help = b
			return err
		}},
		{names: []string{"opt", "optimize"}, assigned: func(c *cliFlags, v string) error {
			level, err := normalizeOptLevel(v)
			c.optLevel = level
			return err
		}},
		{names: []string{"ghc-opt", "ghcopt"}, assigned: func(c *cliFlags, v string) error {
			return c.ghcOpts.Set(v)
		}},
		{names: []string{"ld-opt", "ldopt"}, assigned: func(c *cliFlags, v string) error {
			return c.ldOpts.Set(v)
		}},
		{names: []string{"arg", "args"}, assigned: func(c *cliFlags, v string) error {
			c.runArgs = append(c.runArgs, v)
			return nil
		}},
		{names: []string{"strip"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.strip = b
			return err
		}},
		{names: []string{"time", "timings"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.timings = b
			return err
		}},
		{names: []string{"n", "dry-run", "dryrun"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.dryRun = b
			return err
		}},
		{names: []string{"w", "watch"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.watch = b
			return err
		}},
		{names: []string{"clean"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.clean = b
			return err
		}},
		{names: []string{"init"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.initCmd = b
			return err
		}},
		{names: []string{"f", "force"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.force = b
			return err
		}},
		{names: []string{"doctor"}, boolean: true, assigned: func(c *cliFlags, v string) error {
			b, err := strconv.ParseBool(v)
			c.doctor = b
			return err
		}},
	}
}

func lookupFlagSpec(specs []flagSpec, name string) *flagSpec {
	name = strings.ToLower(name)
	for i := range specs {
		for _, alias := range specs[i].names {
			if alias == name {
				return &specs[i]
			}
		}
	}
	return nil
}

// parseArgs accepts flags before, between and after the positional arguments so
// that both `gmm -o app main.cmm` and `gmm main.cmm -o app` behave the same.
func parseArgs(argv []string) (*cliFlags, error) {
	c := &cliFlags{}
	specs := flagSpecs()
	onlyPositional := false

	for i := 0; i < len(argv); i++ {
		arg := argv[i]

		if onlyPositional || arg == "-" || !strings.HasPrefix(arg, "-") || arg == "" {
			c.positional = append(c.positional, arg)
			continue
		}
		if arg == "--" {
			onlyPositional = true
			continue
		}

		name := strings.TrimLeft(arg, "-")

		var spec *flagSpec
		value := ""
		hasValue := false

		spec = lookupFlagSpec(specs, name)
		if spec != nil {
			if idx := strings.Index(name, "="); idx >= 0 {
				value = name[idx+1:]
				hasValue = true
				name = name[:idx]
			}
		} else {
			base, tail := name, ""
			if idx := strings.Index(base, "="); idx >= 0 {
				base, tail = base[:idx], base[idx+1:]
			}
			spec = lookupFlagSpec(specs, base)
			if spec != nil {
				value, hasValue = tail, true
			} else {
				// Support the attached forms: -oapp and -oC=exe.
				for cut := len(base) - 1; cut > 0 && spec == nil; cut-- {
					candidate := lookupFlagSpec(specs, base[:cut])
					if candidate == nil || candidate.boolean {
						continue
					}
					spec = candidate
					value = base[cut:]
					if tail != "" {
						value += "=" + tail
					}
					hasValue = true
				}
			}
		}
		if spec == nil {
			return nil, fmt.Errorf("unknown flag %q (run 'gmm -h' for the list of flags)", arg)
		}

		if !hasValue {
			if spec.boolean {
				value = "true"
			} else {
				if i+1 >= len(argv) {
					return nil, fmt.Errorf("flag -%s requires a value", name)
				}
				i++
				value = argv[i]
			}
		}

		if err := spec.assigned(c, value); err != nil {
			return nil, fmt.Errorf("invalid value %q for flag -%s: %w", value, name, err)
		}
	}

	return c, nil
}

// ---------------------------------------------------------------------------
// Small helpers
// ---------------------------------------------------------------------------

func trimQuotes(s string) string {
	s = strings.TrimSpace(s)
	if len(s) >= 2 {
		if (s[0] == '"' && s[len(s)-1] == '"') || (s[0] == '\'' && s[len(s)-1] == '\'') {
			return s[1 : len(s)-1]
		}
	}
	return s
}

func splitList(value string) []string {
	var out []string
	for _, part := range strings.Split(value, ",") {
		part = strings.TrimSpace(part)
		if part != "" {
			out = append(out, part)
		}
	}
	return out
}

// splitInputArg picks the C-- source out of the positional arguments and
// returns the remaining ones. The first .cmm file wins, so both
// `gmm -o app main.cmm` and `gmm -o app extra.o main.cmm` work. When no
// argument carries a .cmm extension the first one is used as the input, which
// keeps the previous behaviour for a mistyped extension.
func splitInputArg(args []string) (string, []string) {
	for i, arg := range args {
		if strings.EqualFold(filepath.Ext(arg), ".cmm") {
			rest := make([]string, 0, len(args)-1)
			rest = append(rest, args[:i]...)
			rest = append(rest, args[i+1:]...)
			return arg, rest
		}
	}
	return args[0], args[1:]
}

func isLibraryArg(arg string) bool {
	lower := strings.ToLower(arg)
	return strings.HasSuffix(lower, ".so") || strings.HasSuffix(lower, ".a") ||
		strings.HasSuffix(lower, ".dll") || strings.HasSuffix(lower, ".lib")
}

// normalizeTargetType canonicalises the value of -o C="<target>".
//
// Each target names a concrete output format, so the name always matches the
// file that gets produced:
//
//	exe     Windows PE executable (.exe)
//	dll     Windows PE shared library (.dll)
//	linux   Linux ELF executable (.elf)
//	so      native shared object (.so)
//	native  plain executable for the host, no format implied
func normalizeTargetType(target string) (string, error) {
	t := strings.ToLower(strings.TrimSpace(trimQuotes(target)))
	switch t {
	case "":
		return "native", nil
	case "native", "host":
		return "native", nil
	case "exe", "windows", "pe":
		return "exe", nil
	case "linux", "elf":
		return "linux", nil
	case "so", "shared":
		return "so", nil
	case "dll":
		return "dll", nil
	default:
		return "", fmt.Errorf("unknown target %q (expected one of: native, exe, dll, linux, so)", target)
	}
}

// normalizeOptLevel canonicalises the value of -opt: "2", "O2" and "-O2" all
// select GHC's -O2. An empty value keeps the GHC default.
func normalizeOptLevel(level string) (string, error) {
	l := strings.ToLower(strings.TrimSpace(trimQuotes(level)))
	l = strings.TrimPrefix(strings.TrimPrefix(l, "-"), "o")
	switch l {
	case "", "0", "1", "2":
		return l, nil
	}
	return "", fmt.Errorf("unknown optimization level %q (expected one of: 0, 1, 2)", level)
}

// targetPlatform reports the OS a target belongs to. An empty result means the
// target follows the host.
func targetPlatform(target string) string {
	switch target {
	case "exe", "dll":
		return "windows"
	case "linux":
		return "linux"
	}
	return ""
}

// checkTargetSupported rejects a target whose platform differs from the host.
// A cross build would silently produce a host binary under a foreign name, so
// it is refused instead.
func checkTargetSupported(target string) error {
	want := targetPlatform(target)
	if want == "" || want == runtime.GOOS {
		return nil
	}

	archNote := ""
	if runtime.GOARCH == "amd64" && want == "windows" {
		archNote = " (this needs a Windows-targeting GHC, not the bundled " + runtime.GOOS + " toolchain)"
	}

	return fmt.Errorf("target %q produces %s binaries, but this toolchain targets %s%s\n"+
		"       use -o C=\"native\" for a %s executable, or install a %s GHC and set GMM_GHC",
		target, want, runtime.GOOS, archNote, runtime.GOOS, want)
}

// resolveOutputName appends the extension implied by the selected target so the
// configured output always matches the file the linker really produces.
func resolveOutputName(output, target string) string {
	if output == "" {
		return output
	}
	lower := strings.ToLower(output)
	switch target {
	case "so":
		if !strings.HasSuffix(lower, ".so") && !strings.HasSuffix(lower, ".dylib") {
			output += ".so"
		}
	case "dll":
		if !strings.HasSuffix(lower, ".dll") {
			output += ".dll"
		}
	case "exe":
		if !strings.HasSuffix(lower, ".exe") {
			output += ".exe"
		}
	case "linux":
		if filepath.Ext(output) == "" {
			output += ".elf"
		}
	default:
		if runtime.GOOS == "windows" && !strings.HasSuffix(lower, ".exe") {
			output += ".exe"
		}
	}
	return output
}

func dedupeStrings(in []string) []string {
	seen := make(map[string]bool, len(in))
	out := make([]string, 0, len(in))
	for _, s := range in {
		if s == "" || seen[s] {
			continue
		}
		seen[s] = true
		out = append(out, s)
	}
	return out
}

func executeBinary(binPath string, runArgs []string) int {
	cleanPath := filepath.Clean(binPath)
	if runtime.GOOS == "windows" {
		if !strings.HasSuffix(strings.ToLower(cleanPath), ".exe") &&
			!strings.HasSuffix(strings.ToLower(cleanPath), ".dll") {
			if _, err := os.Stat(cleanPath + ".exe"); err == nil {
				cleanPath += ".exe"
			}
		}
	} else if !strings.ContainsRune(cleanPath, os.PathSeparator) && !strings.HasPrefix(cleanPath, ".") {
		cleanPath = "." + string(os.PathSeparator) + cleanPath
	}

	if echoCommand(cleanPath, runArgs) {
		return 0
	}

	if _, err := os.Stat(cleanPath); err != nil {
		fmt.Fprintf(os.Stderr, "error: cannot run %s: %v\n", binPath, err)
		return 127
	}

	cmd := exec.Command(cleanPath, runArgs...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Stdin = os.Stdin

	// Ctrl-C is meant for the program being run: wait for it instead of dying
	// first, so gmm can still report its exit code and clean up after it.
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, os.Interrupt)
	defer signal.Stop(sig)

	if err := cmd.Run(); err != nil {
		var exitErr *exec.ExitError
		if errors.As(err, &exitErr) {
			if code := exitErr.ExitCode(); code >= 0 {
				return code
			}
			return 1
		}
		fmt.Fprintf(os.Stderr, "error: cannot run %s: %v\n", binPath, err)
		return 127
	}
	return 0
}

// runFilteredGHC runs GHC and synchronously filters libtinfo warnings from Stderr
func runFilteredGHC(binPath string, args ...string) error {
	if echoCommand(binPath, args) {
		return nil
	}

	cmd := exec.Command(binPath, args...)
	cmd.Stdout = os.Stdout

	stderrPipe, err := cmd.StderrPipe()
	if err != nil {
		return err
	}

	if err := cmd.Start(); err != nil {
		return err
	}

	scanner := bufio.NewScanner(stderrPipe)
	scanner.Buffer(make([]byte, 64*1024), 4*1024*1024)
	for scanner.Scan() {
		line := scanner.Text()
		if strings.Contains(line, "libtinfo.so") && strings.Contains(line, "no version information available") {
			continue
		}
		fmt.Fprintln(os.Stderr, line)
	}
	scanErr := scanner.Err()

	if err := cmd.Wait(); err != nil {
		return err
	}
	return scanErr
}

// runTool streams the output of an external tool and remembers its stderr so a
// confusing linker message can be turned into an actionable hint.
func runTool(name string, args ...string) error {
	if echoCommand(name, args) {
		return nil
	}

	var stderr strings.Builder
	cmd := exec.Command(name, args...)
	cmd.Stdout = os.Stdout
	cmd.Stderr = io.MultiWriter(os.Stderr, &stderr)
	err := cmd.Run()
	if err != nil {
		reportLinkHint(stderr.String())
	}
	return err
}

func reportLinkHint(stderrText string) {
	lower := strings.ToLower(stderrText)
	if strings.Contains(lower, "can not be used when making a shared object") ||
		strings.Contains(lower, "recompile with -fpic") {
		fmt.Fprintln(os.Stderr, "hint: the GHC code generator did not emit position independent code.")
		fmt.Fprintln(os.Stderr, "      Rebuild the shared object with the LLVM backend: gmm -llvm -o C=\"so\" ...")
	}
}

func extractEmbeddedLibs() (string, error) {
	tempDir, err := os.MkdirTemp("", "gmm_lib_*")
	if err != nil {
		return "", err
	}

	err = fs.WalkDir(libFS, "libgmm", func(path string, d fs.DirEntry, err error) error {
		if err != nil {
			return err
		}
		if d.IsDir() {
			return nil
		}
		relPath, relErr := filepath.Rel("libgmm", path)
		if relErr != nil {
			return relErr
		}
		outPath := filepath.Join(tempDir, relPath)

		if err := os.MkdirAll(filepath.Dir(outPath), 0755); err != nil {
			return err
		}

		data, err := libFS.ReadFile(path)
		if err != nil {
			return err
		}

		return os.WriteFile(outPath, data, 0644)
	})
	if err != nil {
		os.RemoveAll(tempDir)
		return "", err
	}

	return tempDir, nil
}

func fileExists(path string) bool {
	info, err := os.Stat(path)
	return err == nil && !info.IsDir()
}

func trackArtifact(path string, created *[]Artifact) {
	_, err := os.Stat(path)
	*created = append(*created, Artifact{Path: path, Preexisted: err == nil})
}

func cleanupArtifacts(artifacts []Artifact, keep bool) {
	for _, a := range artifacts {
		if keep {
			continue
		}
		if a.Preexisted {
			continue
		}
		if err := os.Remove(a.Path); err != nil && !os.IsNotExist(err) {
			fmt.Fprintf(os.Stderr, "warning: could not remove %s: %v\n", a.Path, err)
		}
	}
}

// ---------------------------------------------------------------------------
// conf.hmm
// ---------------------------------------------------------------------------

func findConfigFile(inputFile string) string {
	var dirs []string
	if inputFile != "" {
		dirs = append(dirs, filepath.Dir(inputFile))
	}
	dirs = append(dirs, ".")

	for _, dir := range dirs {
		candidate := filepath.Join(dir, confFileName)
		if fileExists(candidate) {
			return candidate
		}
	}
	return ""
}

// parseBoolValue merges a boolean conf.hmm key into dst. A key can only turn an
// option on: an option already enabled on the command line (-llvm, -br, ...)
// is never switched off again by a "false" stored in conf.hmm.
func parseBoolValue(key, val string, dst *bool) error {
	var on bool
	switch strings.ToLower(val) {
	case "1", "true", "yes", "on":
		on = true
	case "0", "false", "no", "off":
		on = false
	default:
		return fmt.Errorf("%s expects a boolean value, got %q", key, val)
	}
	*dst = *dst || on
	return nil
}

func parseHMMFile(filename string, cfg *Config) error {
	if filename == "" {
		return nil
	}
	file, err := os.Open(filename)
	if err != nil {
		if os.IsNotExist(err) {
			return nil
		}
		return err
	}
	defer file.Close()

	targetSet := false
	scanner := bufio.NewScanner(file)
	scanner.Buffer(make([]byte, 64*1024), 1024*1024)
	line := 0
	for scanner.Scan() {
		line++
		text := strings.TrimSpace(scanner.Text())
		if text == "" || strings.HasPrefix(text, "#") || strings.HasPrefix(text, ";") {
			continue
		}
		parts := strings.SplitN(text, "=", 2)
		if len(parts) != 2 {
			return fmt.Errorf("%s:%d: expected key=value, got %q", filename, line, text)
		}
		key := strings.ToLower(strings.TrimSpace(parts[0]))
		val := trimQuotes(parts[1])

		var keyErr error
		switch key {
		case "input", "src", "source":
			if cfg.InputFile == "" {
				cfg.InputFile = val
			}
		case "output", "out", "bin":
			if !cfg.OutputSet {
				cfg.Output = val
				cfg.OutputSet = val != ""
			}
		case "target", "type", "format":
			target, terr := normalizeTargetType(val)
			if terr != nil {
				keyErr = fmt.Errorf("%s:%d: %v", filename, line, terr)
				break
			}
			if target != "" {
				cfg.TargetType = target
				targetSet = true
			}
		case "libs", "libraries":
			cfg.Libs = append(cfg.Libs, splitList(val)...)
		case "cfiles", "csources", "csrc":
			cfg.CFiles = append(cfg.CFiles, splitList(val)...)
		case "includes", "incdirs":
			cfg.Includes = append(cfg.Includes, splitList(val)...)
		case "defines", "defs":
			cfg.Defines = append(cfg.Defines, splitList(val)...)
		case "objs", "objects", "extraobjs":
			cfg.ExtraObjs = append(cfg.ExtraObjs, splitList(val)...)
		case "keepobj", "keep-obj":
			keyErr = parseBoolValue(key, val, &cfg.KeepObj)
		case "emitasm", "asm":
			keyErr = parseBoolValue(key, val, &cfg.EmitASM)
		case "llvm":
			keyErr = parseBoolValue(key, val, &cfg.UseLLVM)
		case "rts":
			keyErr = parseBoolValue(key, val, &cfg.UseRTS)
		case "run":
			keyErr = parseBoolValue(key, val, &cfg.RunAfter)
		case "strip":
			keyErr = parseBoolValue(key, val, &cfg.Strip)
		case "opt", "optimize":
			if cfg.OptLevel == "" {
				level, lerr := normalizeOptLevel(val)
				if lerr != nil {
					keyErr = fmt.Errorf("%s:%d: %v", filename, line, lerr)
					break
				}
				cfg.OptLevel = level
			}
		case "ghcopts", "ghc-opts":
			cfg.GHCOpts = append(cfg.GHCOpts, splitList(val)...)
		case "ldopts", "ld-opts":
			cfg.LDOpts = append(cfg.LDOpts, splitList(val)...)
		case "args", "runargs":
			// Arguments given with -arg on the command line win.
			if len(cfg.RunArgs) == 0 {
				cfg.RunArgs = splitList(val)
			}
		default:
			// Unknown keys are ignored so newer config files stay loadable.
		}
		if keyErr != nil {
			return keyErr
		}
	}

	if err := scanner.Err(); err != nil {
		return err
	}

	if targetSet {
		if cfg.OutputSet {
			cfg.Output = resolveOutputName(cfg.Output, cfg.TargetType)
		} else if cfg.InputFile != "" {
			base := strings.TrimSuffix(filepath.Base(cfg.InputFile), filepath.Ext(cfg.InputFile))
			cfg.Output = resolveOutputName(base, cfg.TargetType)
			cfg.OutputSet = true
		}
	}
	return nil
}

// ---------------------------------------------------------------------------
// Compilation / linking
// ---------------------------------------------------------------------------

func baseNameWithoutExt(path string) string {
	base := filepath.Base(path)
	return strings.TrimSuffix(base, filepath.Ext(base))
}

func needsCMath(src string) bool {
	data, err := os.ReadFile(src)
	if err != nil {
		return false
	}
	content := string(data)
	return strings.Contains(content, "cmmath.h") || strings.Contains(content, "cmmath.c")
}

// mathObjPath is where the cmmath object of a build is written. It avoids
// clobbering the C-- object when the input itself is named cmmath.cmm, which
// would otherwise produce duplicate symbol errors.
func mathObjPath(inputDir, baseName string) string {
	name := "cmmath.o"
	if baseName == "cmmath" {
		name = "cmmath_lib.o"
	}
	return filepath.Join(inputDir, name)
}

func compileMathLib(cc, tempDir, output string) error {
	src := filepath.Join(tempDir, "cmmath.c")
	if !fileExists(src) {
		return fmt.Errorf("cmmath.c is missing from the embedded library (looked in %s)", tempDir)
	}
	if err := runTool(cc, "-c", src, "-I"+tempDir, "-o", output); err != nil {
		return fmt.Errorf("failed to build cmmath: %w", err)
	}
	return nil
}

func compileCFiles(cc string, cFiles []string, tempDir string, extraIncludes []string, defines []string, shared bool, created *[]Artifact) ([]string, error) {
	var objs []string

	for _, cFile := range cFiles {
		cleanPath := filepath.Clean(cFile)
		if !fileExists(cleanPath) {
			return nil, fmt.Errorf("C source not found: %s", cleanPath)
		}
		baseName := baseNameWithoutExt(cleanPath)
		objName := filepath.Join(filepath.Dir(cleanPath), baseName+"_c.o")

		args := []string{"-c", cleanPath, "-I" + tempDir}
		for _, inc := range extraIncludes {
			if strings.TrimSpace(inc) != "" {
				args = append(args, "-I"+inc)
			}
		}
		for _, def := range defines {
			if strings.TrimSpace(def) != "" {
				args = append(args, "-D"+def)
			}
		}
		if shared {
			args = append(args, "-fPIC")
		}
		args = append(args, "-o", objName)

		trackArtifact(objName, created)
		if err := runTool(cc, args...); err != nil {
			return nil, fmt.Errorf("failed to compile %s: %w", cleanPath, err)
		}
		objs = append(objs, objName)
	}

	return objs, nil
}

func ghcCommonArgs(includePath string, cfg Config) []string {
	args := []string{"-no-hs-main"}

	if cfg.UseLLVM {
		args = append(args, "-fllvm")
	}
	if cfg.OptLevel != "" {
		args = append(args, "-O"+cfg.OptLevel)
	}
	if cfg.IsShared() {
		args = append(args, "-fPIC")
	}

	args = append(args, "-I"+includePath)
	for _, inc := range cfg.Includes {
		if strings.TrimSpace(inc) != "" {
			args = append(args, "-I"+inc)
		}
	}
	for _, def := range cfg.Defines {
		if strings.TrimSpace(def) != "" {
			// The C-- preprocessor is driven by -optc, not -optP/-D, which
			// only reach the Haskell preprocessor.
			args = append(args, "-optc", "-D"+def)
		}
	}
	args = append(args, cfg.GHCOpts...)
	return args
}

func compileToASM(ghc, input, output, includePath string, cfg Config) error {
	if err := ensureDir(filepath.Dir(output)); err != nil {
		return err
	}

	args := append([]string{"-S"}, ghcCommonArgs(includePath, cfg)...)
	args = append(args, input, "-o", output)

	return runFilteredGHC(ghc, args...)
}

func compileWithGHC(ghc, input, output, includePath string, cfg Config) error {
	if err := ensureDir(filepath.Dir(output)); err != nil {
		return err
	}

	args := append([]string{"-c"}, ghcCommonArgs(includePath, cfg)...)
	args = append(args, input, "-o", output)

	return runFilteredGHC(ghc, args...)
}

func linkArgFor(lib string) string {
	lib = strings.TrimSpace(lib)
	if lib == "" {
		return ""
	}
	if strings.HasPrefix(lib, "-") || strings.ContainsAny(lib, `/\`) ||
		strings.HasSuffix(strings.ToLower(lib), ".so") ||
		strings.HasSuffix(strings.ToLower(lib), ".a") ||
		strings.HasSuffix(strings.ToLower(lib), ".dll") ||
		strings.HasSuffix(strings.ToLower(lib), ".lib") {
		return lib
	}
	return "-l" + lib
}

func linkWithGHC(ghc string, objs []string, output string, libs []string, cfg Config) error {
	args := []string{"-no-hs-main"}
	args = append(args, objs...)
	args = append(args, "-o", output)

	if cfg.IsShared() {
		args = append(args, "-optl-fPIC")
	}
	for _, l := range libs {
		if arg := linkArgFor(l); arg != "" {
			args = append(args, "-optl"+arg)
		}
	}
	if runtime.GOOS != "windows" {
		args = append(args, "-optl-lm", "-optl-lc")
	}
	if linker := defaultLinkerKind(); linker != "" {
		args = append(args, "-optl-fuse-ld="+linker)
	}
	if cfg.Strip {
		args = append(args, "-optl-s")
	}
	args = append(args, cfg.GHCOpts...)
	for _, opt := range cfg.LDOpts {
		args = append(args, "-optl"+opt)
	}

	return runFilteredGHC(ghc, args...)
}

// defaultLinkerKind returns a usable `ld` flavour when the GHC bindist asks for
// one that the host does not ship (gold is frequently missing on minimal hosts).
func defaultLinkerKind() string {
	if runtime.GOOS == "windows" {
		return ""
	}
	if lookupInPath("ld.gold") != "" {
		return ""
	}
	if lookupInPath("ld.bfd") != "" {
		return "bfd"
	}
	if lookupInPath("ld.lld") != "" {
		return "lld"
	}
	return ""
}

func linkWithCC(cc string, objs []string, output string, libs []string, cfg Config) error {
	if strings.TrimSpace(cc) == "" {
		return errors.New("no C compiler available for linking (set GMM_CC or install gcc/clang)")
	}

	target := cfg.TargetType
	args := append([]string{}, objs...)

	if target == "so" || target == "dll" {
		args = append(args, "-shared", "-fPIC")
	}

	if runtime.GOOS == "windows" {
		args = append(args, "-o", output, "-g")
	} else {
		if target != "so" && target != "dll" {
			args = append(args, "-no-pie")
		}
		args = append(args, "-o", output, "-g")
	}

	for _, l := range libs {
		if arg := linkArgFor(l); arg != "" {
			args = append(args, arg)
		}
	}

	if runtime.GOOS != "windows" {
		args = append(args, "-lm", "-lc")
	}
	if cfg.Strip {
		args = append(args, "-s")
	}
	args = append(args, cfg.LDOpts...)

	return runTool(cc, args...)
}

// ---------------------------------------------------------------------------
// Build driver
// ---------------------------------------------------------------------------

func emitAssembly(cfg Config, tp ToolPaths, tempDir, inputDir, baseName string) error {
	var created []Artifact

	asmOutput := filepath.Join(inputDir, baseName+".s")
	if cfg.OutputSet && cfg.Output != "" && cfg.Output != DefaultTargetOutput {
		asmOutput = cfg.Output
	}
	trackArtifact(asmOutput, &created)
	if cerr := compileToASM(tp.GHC, cfg.InputFile, asmOutput, tempDir, cfg); cerr != nil {
		cleanupArtifacts(created, false)
		return toolFailure("generating assembly failed", cerr)
	}
	fmt.Printf("Assembly output saved to %s\n", asmOutput)
	return nil
}

// buildOnce compiles and links the project one time. Every failure is returned
// instead of exiting, so the caller can clean up and -watch can keep going.
func buildOnce(cfg Config, tp ToolPaths, tempDir, inputDir, baseName string, sw *Stopwatch) (err error) {
	if cfg.EmitASM {
		err = emitAssembly(cfg, tp, tempDir, inputDir, baseName)
		sw.Lap("assembly")
		return err
	}

	var created []Artifact
	defer func() {
		// A failed build never leaves half written objects behind, even
		// with -keep-obj.
		cleanupArtifacts(created, err == nil && cfg.KeepObj)
	}()

	mainObjFile := filepath.Join(inputDir, baseName+".o")
	trackArtifact(mainObjFile, &created)
	if cerr := compileWithGHC(tp.GHC, cfg.InputFile, mainObjFile, tempDir, cfg); cerr != nil {
		if !isExitError(cerr) {
			return toolFailure("running GHC failed", cerr)
		}
		if cfg.UseLLVM {
			return fmt.Errorf("compiling with the LLVM backend failed\n"+
				"hint: GHC needs LLVM %s; install it or point GMM_LLVM at a working 'opt' binary", llvmVersionRange)
		}
		return errReported
	}
	sw.Lap("compile")

	compiledCObjs, cerr := compileCFiles(tp.CC, cfg.CFiles, tempDir, cfg.Includes, cfg.Defines, cfg.IsShared(), &created)
	if cerr != nil {
		return cerr
	}
	sw.Lap("c files")

	linkObjs := []string{mainObjFile}
	linkObjs = append(linkObjs, compiledCObjs...)

	if needsCMath(cfg.InputFile) {
		mathObj := mathObjPath(inputDir, baseName)
		trackArtifact(mathObj, &created)
		if merr := compileMathLib(tp.CC, tempDir, mathObj); merr != nil {
			return merr
		}
		linkObjs = append(linkObjs, mathObj)
		sw.Lap("cmmath")
	}

	linkObjs = append(linkObjs, cfg.ExtraObjs...)

	if derr := ensureDir(filepath.Dir(cfg.Output)); derr != nil {
		return fmt.Errorf("creating output directory: %v", derr)
	}

	if cfg.UseRTS {
		if tp.GHC == "" {
			return errors.New("-rts requested but no GHC binary is available")
		}
		if lerr := linkWithGHC(tp.GHC, linkObjs, cfg.Output, cfg.Libs, cfg); lerr != nil {
			return toolFailure("linking with GHC failed", lerr)
		}
	} else {
		if strings.TrimSpace(tp.CC) == "" {
			return errors.New("no C compiler available for linking (set GMM_CC or install gcc/clang)")
		}
		if lerr := linkWithCC(tp.CC, linkObjs, cfg.Output, cfg.Libs, cfg); lerr != nil {
			return toolFailure("linking failed", lerr)
		}
	}
	sw.Lap("link")
	return nil
}

// buildAndRun builds once, prints the timings and runs the result when asked.
// It returns the exit code gmm (or the program it ran) should finish with.
func buildAndRun(cfg Config, tp ToolPaths, tempDir, inputDir, baseName string) int {
	sw := newStopwatch(cfg.Timings)
	if err := buildOnce(cfg, tp, tempDir, inputDir, baseName, sw); err != nil {
		if !errors.Is(err, errReported) {
			fmt.Fprintf(os.Stderr, "error: %v\n", err)
		}
		return 1
	}
	sw.Report()

	if cfg.RunAfter && !cfg.EmitASM {
		if cfg.IsShared() {
			fmt.Fprintf(os.Stderr, "warning: %s is a shared object, not running it\n", cfg.Output)
			return 0
		}
		return executeBinary(cfg.Output, cfg.RunArgs)
	}
	return 0
}

// ---------------------------------------------------------------------------
// Project commands: clean, init, doctor, watch
// ---------------------------------------------------------------------------

// projectArtifacts lists every file a build of cfg may leave behind: the
// objects next to the input and the output itself. `gmm clean` removes them.
func projectArtifacts(cfg Config, inputDir, baseName string) []string {
	files := []string{filepath.Join(inputDir, baseName+".o")}
	if needsCMath(cfg.InputFile) {
		files = append(files, mathObjPath(inputDir, baseName))
	}
	for _, cFile := range cfg.CFiles {
		cleanPath := filepath.Clean(cFile)
		files = append(files, filepath.Join(filepath.Dir(cleanPath), baseNameWithoutExt(cleanPath)+"_c.o"))
	}
	if cfg.EmitASM {
		files = append(files, filepath.Join(inputDir, baseName+".s"))
	}
	if cfg.Output != "" {
		files = append(files, cfg.Output)
	}
	return dedupeStrings(files)
}

func cleanProject(cfg Config, inputDir, baseName string) int {
	// Sources and user supplied objects are never touched.
	protected := map[string]bool{filepath.Clean(cfg.InputFile): true}
	for _, p := range cfg.CFiles {
		protected[filepath.Clean(p)] = true
	}
	for _, p := range cfg.ExtraObjs {
		protected[filepath.Clean(p)] = true
	}

	removed := 0
	for _, path := range projectArtifacts(cfg, inputDir, baseName) {
		if protected[filepath.Clean(path)] || !fileExists(path) {
			continue
		}
		if execMode.DryRun {
			fmt.Println("[dry-run] remove", path)
			removed++
			continue
		}
		if err := os.Remove(path); err != nil {
			fmt.Fprintf(os.Stderr, "warning: could not remove %s: %v\n", path, err)
			continue
		}
		fmt.Println("Removed", path)
		removed++
	}
	if removed == 0 {
		fmt.Println("Nothing to clean")
	}
	return 0
}

// renderConfFile turns the current configuration into conf.hmm text, using the
// same keys parseHMMFile understands.
func renderConfFile(cfg Config) string {
	var b strings.Builder
	fmt.Fprintf(&b, "# %s generated by gmm %s\n", confFileName, Version)

	str := func(key, val string) {
		if val != "" {
			fmt.Fprintf(&b, "%s=%s\n", key, val)
		}
	}
	list := func(key string, vals []string) {
		if len(vals) > 0 {
			fmt.Fprintf(&b, "%s=%s\n", key, strings.Join(vals, ","))
		}
	}
	toggle := func(key string, on bool) {
		if on {
			fmt.Fprintf(&b, "%s=true\n", key)
		}
	}

	str("input", cfg.InputFile)
	if cfg.OutputSet {
		str("output", cfg.Output)
	}
	str("target", cfg.TargetType)
	str("opt", cfg.OptLevel)
	list("libs", cfg.Libs)
	list("includes", cfg.Includes)
	list("defines", cfg.Defines)
	list("cfiles", cfg.CFiles)
	list("objs", cfg.ExtraObjs)
	list("ghcopts", cfg.GHCOpts)
	list("ldopts", cfg.LDOpts)
	list("args", cfg.RunArgs)
	toggle("keepobj", cfg.KeepObj)
	toggle("emitasm", cfg.EmitASM)
	toggle("llvm", cfg.UseLLVM)
	toggle("rts", cfg.UseRTS)
	toggle("strip", cfg.Strip)
	toggle("run", cfg.RunAfter)
	return b.String()
}

func initProject(cfg Config, force bool) int {
	if fileExists(confFileName) && !force {
		fmt.Fprintf(os.Stderr, "error: %s already exists (use -force to overwrite it)\n", confFileName)
		return 1
	}

	content := renderConfFile(cfg)
	if execMode.DryRun {
		fmt.Print(content)
		return 0
	}
	if err := os.WriteFile(confFileName, []byte(content), 0644); err != nil {
		fmt.Fprintf(os.Stderr, "error: writing %s: %v\n", confFileName, err)
		return 1
	}
	fmt.Println("Wrote", confFileName)
	return 0
}

// locateInstalledTools finds an already installed toolchain without touching
// the network or the disk. The second value says where it was found.
func locateInstalledTools() (ToolPaths, string) {
	if ghc := strings.TrimSpace(os.Getenv("GMM_GHC")); ghc != "" {
		tp := ToolPaths{
			GHC:  ghc,
			CC:   strings.TrimSpace(os.Getenv("GMM_CC")),
			LLVM: strings.TrimSpace(os.Getenv("GMM_LLVM")),
		}
		if tp.CC == "" && isExecutableFile(ghc) {
			tp.CC = guessCC(ghc)
		}
		if tp.LLVM == "" {
			tp.LLVM = lookupInPath("opt", "opt.exe")
		}
		return tp, "the GMM_GHC environment variable"
	}

	// Same order as setupEnvironment: a system GHC wins over the bundled one.
	if tp, ok := systemGHCToolPaths(); ok {
		return tp, "the system package manager (PATH)"
	}

	for _, dir := range installDirCandidates() {
		if tp, err := loadToolPathsFrom(dir); err == nil {
			if tp.CC == "" && isExecutableFile(tp.GHC) {
				tp.CC = guessCC(tp.GHC)
			}
			if tp.LLVM == "" {
				tp.LLVM = lookupInPath("opt", "opt.exe")
			}
			return tp, sitFilePathIn(dir)
		}
		ghcDir := filepath.Join(dir, "Ghc")
		if tp, ok := findToolsInDir(ghcDir); ok {
			return tp, ghcDir
		}
	}
	return ToolPaths{}, ""
}

// toolVersionLine runs `<tool> <arg>` and returns the most informative line of
// its output, or "" when the tool cannot be started.
func toolVersionLine(path, arg string) string {
	ctx, cancel := context.WithTimeout(context.Background(), 15*time.Second)
	defer cancel()

	out, err := exec.CommandContext(ctx, path, arg).CombinedOutput()
	if err != nil && len(out) == 0 {
		return ""
	}
	first := ""
	for _, line := range strings.Split(string(out), "\n") {
		line = strings.TrimSpace(line)
		if line == "" {
			continue
		}
		if first == "" {
			first = line
		}
		if strings.Contains(strings.ToLower(line), "version") {
			return line
		}
	}
	return first
}

func runDoctor() int {
	problems := 0

	fmt.Printf("gmm %s (%s/%s)\n\n", Version, runtime.GOOS, runtime.GOARCH)

	report := func(label, path, versionArg string, required bool) {
		if path == "" {
			if required {
				fmt.Printf("  [FAIL] %-5s not found\n", label)
				problems++
			} else {
				fmt.Printf("  [ -- ] %-5s not found (optional, only needed for -llvm)\n", label)
			}
			return
		}
		if !isExecutableFile(path) {
			fmt.Printf("  [FAIL] %-5s %s (not an executable file)\n", label, path)
			problems++
			return
		}
		fmt.Printf("  [ OK ] %-5s %s\n", label, path)
		if v := toolVersionLine(path, versionArg); v != "" {
			fmt.Printf("                %s\n", v)
		}
	}

	tp, source := locateInstalledTools()
	if source == "" {
		fmt.Println("Toolchain: not installed yet (gmm downloads GHC on the first build)")
		if _, err := ghcDownloadURL(); err != nil {
			fmt.Printf("  [FAIL] automatic download is unavailable: %v\n", err)
			problems++
		}
	} else {
		fmt.Printf("Toolchain (from %s):\n", source)
		report("GHC", tp.GHC, "--numeric-version", true)
		report("CC", tp.CC, "--version", true)
		report("LLVM", tp.LLVM, "--version", false)
	}

	fmt.Println("\nEnvironment:")
	for _, name := range []string{"GMM_HOME", "GMM_GHC", "GMM_CC", "GMM_LLVM"} {
		val := strings.TrimSpace(os.Getenv(name))
		if val == "" {
			val = "(not set)"
		}
		fmt.Printf("  %-9s %s\n", name, val)
	}
	fmt.Println("  install dirs:", strings.Join(installDirCandidates(), ", "))

	if runtime.GOOS != "windows" {
		fmt.Println("\nLinkers:")
		for _, name := range []string{"ld.gold", "ld.bfd", "ld.lld"} {
			state := "missing"
			if lookupInPath(name) != "" {
				state = "found"
			}
			fmt.Printf("  %-8s %s\n", name, state)
		}
		if kind := defaultLinkerKind(); kind != "" {
			fmt.Printf("  -rts links with -fuse-ld=%s (ld.gold is not available)\n", kind)
		}
	}

	fmt.Println("\nTargets:")
	for _, target := range []string{"native", "exe", "dll", "linux", "so"} {
		state := "supported"
		if checkTargetSupported(target) != nil {
			state = "unavailable on " + runtime.GOOS + " (needs a " + targetPlatform(target) + " toolchain)"
		}
		fmt.Printf("  %-7s %s\n", target, state)
	}

	embedded := 0
	_ = fs.WalkDir(libFS, "libgmm", func(path string, d fs.DirEntry, err error) error {
		if err == nil && !d.IsDir() {
			embedded++
		}
		return nil
	})
	fmt.Printf("\nEmbedded library files: %d\n", embedded)

	if problems > 0 {
		fmt.Printf("\n%d problem(s) found\n", problems)
		return 1
	}
	fmt.Println("\nEverything looks fine")
	return 0
}

var includeRe = regexp.MustCompile(`(?m)^\s*#\s*include\s+"([^"]+)"`)

// collectIncludes follows the quoted #include lines of path and records every
// header that exists on disk, so a watcher notices header edits too.
func collectIncludes(path string, dirs []string, seen map[string]bool) {
	data, err := os.ReadFile(path)
	if err != nil {
		return
	}
	for _, m := range includeRe.FindAllStringSubmatch(string(data), -1) {
		for _, dir := range append([]string{filepath.Dir(path)}, dirs...) {
			candidate := filepath.Clean(filepath.Join(dir, m[1]))
			if !fileExists(candidate) {
				continue
			}
			if !seen[candidate] {
				seen[candidate] = true
				collectIncludes(candidate, dirs, seen)
			}
			break
		}
	}
}

// watchStamps fingerprints every file the build depends on (modification time
// and size), so two calls differ exactly when something was edited.
func watchStamps(cfg Config, confPath string) map[string]string {
	files := map[string]bool{}
	add := func(p string) {
		if p != "" && fileExists(p) {
			files[filepath.Clean(p)] = true
		}
	}
	add(cfg.InputFile)
	add(confPath)
	for _, p := range cfg.CFiles {
		add(p)
	}
	for _, p := range cfg.ExtraObjs {
		add(p)
	}
	for _, src := range append([]string{cfg.InputFile}, cfg.CFiles...) {
		collectIncludes(src, cfg.Includes, files)
	}

	stamps := make(map[string]string, len(files))
	for path := range files {
		if info, err := os.Stat(path); err == nil {
			stamps[path] = fmt.Sprintf("%d/%d", info.ModTime().UnixNano(), info.Size())
		}
	}
	return stamps
}

func sameStamps(a, b map[string]string) bool {
	if len(a) != len(b) {
		return false
	}
	for path, stamp := range a {
		if b[path] != stamp {
			return false
		}
	}
	return true
}

// watchProject rebuilds (and runs, with -r) every time a watched file changes
// until Ctrl-C. A failed build is reported and the watcher keeps waiting.
func watchProject(cfg Config, confPath string, tp ToolPaths, tempDir, inputDir, baseName string, interrupt <-chan os.Signal) int {
	fmt.Printf("Watching %s (Ctrl-C to stop)\n", cfg.InputFile)

	var last map[string]string
	for {
		current := watchStamps(cfg, confPath)
		if last == nil || !sameStamps(last, current) {
			if last != nil {
				// Give editors a moment to finish writing the file.
				time.Sleep(150 * time.Millisecond)
				current = watchStamps(cfg, confPath)
			}
			last = current

			fmt.Printf("\n[%s] building %s\n", time.Now().Format("15:04:05"), cfg.InputFile)
			if code := buildAndRun(cfg, tp, tempDir, inputDir, baseName); code != 0 {
				fmt.Fprintf(os.Stderr, "[%s] finished with exit status %d\n", time.Now().Format("15:04:05"), code)
			}
		}

		select {
		case <-interrupt:
			fmt.Println("\nwatch stopped")
			return 0
		case <-time.After(500 * time.Millisecond):
		}
	}
}

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------

func fail(format string, a ...interface{}) {
	fmt.Fprintf(os.Stderr, "error: "+format+"\n", a...)
	os.Exit(1)
}

func main() {
	flags, err := parseArgs(os.Args[1:])
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		fmt.Fprintln(os.Stderr, "run 'gmm -h' for usage information")
		os.Exit(2)
	}

	if flags.help {
		printHelp()
		return
	}
	if flags.version {
		printVersion()
		return
	}

	execMode.Verbose = flags.verbose
	execMode.DryRun = flags.dryRun

	if flags.doctor || (len(flags.positional) > 0 && strings.EqualFold(flags.positional[0], "doctor")) {
		os.Exit(runDoctor())
	}

	isBuildMode := flags.buildCmd || flags.buildClean || flags.buildRun || flags.watch
	isOnlyRunMode := flags.runCmd && !isBuildMode
	isCleanMode := flags.clean
	isInitMode := flags.initCmd

	cfg := Config{
		EmitASM:    flags.emitASM,
		KeepObj:    flags.keepObj,
		UseLLVM:    flags.useLLVM,
		UseRTS:     flags.useRTS,
		CleanBuild: flags.buildClean,
		RunAfter:   flags.buildRun || (flags.runCmd && isBuildMode),
		Verbose:    flags.verbose,
		Strip:      flags.strip,
		Timings:    flags.timings,
		Watch:      flags.watch,
		OptLevel:   flags.optLevel,
		GHCOpts:    append([]string{}, flags.ghcOpts...),
		LDOpts:     append([]string{}, flags.ldOpts...),
		RunArgs:    append([]string{}, flags.runArgs...),
	}
	cfg.Includes = append(cfg.Includes, flags.includes...)
	cfg.Defines = append(cfg.Defines, flags.defines...)
	cfg.Libs = append(cfg.Libs, flags.libs...)

	for _, t := range flags.targetTypes {
		target, terr := normalizeTargetType(t)
		if terr != nil {
			fail("%v", terr)
		}
		cfg.TargetType = target
	}

	args := flags.positional
	if len(args) > 0 {
		switch strings.ToLower(args[0]) {
		case "build", "b":
			isBuildMode = true
			args = args[1:]
		case "bc":
			flags.buildClean = true
			cfg.CleanBuild = true
			isBuildMode = true
			args = args[1:]
		case "run", "r":
			if !isBuildMode {
				isOnlyRunMode = true
			}
			args = args[1:]
		case "br":
			flags.buildRun = true
			cfg.RunAfter = true
			isBuildMode = true
			args = args[1:]
		case "watch", "w":
			isBuildMode = true
			cfg.Watch = true
			if flags.runCmd {
				cfg.RunAfter = true
				isOnlyRunMode = false
			}
			args = args[1:]
		case "clean":
			isCleanMode = true
			args = args[1:]
		case "init":
			isInitMode = true
			args = args[1:]
		}
	}

	// Detect inline C="<target>" if passed positionally
	if len(args) > 0 && strings.HasPrefix(args[0], "C=") {
		target, terr := normalizeTargetType(strings.TrimPrefix(args[0], "C="))
		if terr != nil {
			fail("%v", terr)
		}
		if target != "" {
			cfg.TargetType = target
		}
		args = args[1:]
	}

	// Command line values win over the ones stored in conf.hmm.
	cliLibs := append([]string{}, cfg.Libs...)
	cliIncludes := append([]string{}, cfg.Includes...)
	cliDefines := append([]string{}, cfg.Defines...)
	if flags.outputSeen && strings.TrimSpace(flags.outputRaw) != "" {
		cfg.Output = strings.TrimSpace(flags.outputRaw)
		cfg.OutputSet = true
	}

	cliInput := ""
	if len(args) > 0 {
		cliInput = args[0]
	}

	confPath := findConfigFile(cliInput)
	if confPath == "" {
		confPath = findConfigFile("")
	}
	if isInitMode {
		// init writes a fresh conf.hmm; the old one must not leak into it.
		confPath = ""
	}
	if confPath != "" && cfg.Verbose {
		fmt.Println("Using configuration file", confPath)
	}
	if err := parseHMMFile(confPath, &cfg); err != nil {
		fail("%v", err)
	}

	cfg.Includes = append(cliIncludes, cfg.Includes...)
	cfg.Defines = append(cliDefines, cfg.Defines...)
	cfg.Libs = append(cliLibs, cfg.Libs...)

	if isOnlyRunMode {
		targetBin := cfg.Output
		if !cfg.OutputSet || targetBin == "" {
			targetBin = DefaultTargetOutput
		}
		if len(args) > 0 {
			targetBin = args[0]
		}
		os.Exit(executeBinary(targetBin, cfg.RunArgs))
	}

	if len(args) > 0 {
		input, rest := splitInputArg(args)
		cfg.InputFile = input
		for _, arg := range rest {
			cleanArg := filepath.Clean(arg)
			if strings.HasPrefix(cleanArg, "C=") {
				target, terr := normalizeTargetType(strings.TrimPrefix(cleanArg, "C="))
				if terr != nil {
					fail("%v", terr)
				}
				if target != "" {
					cfg.TargetType = target
				}
			} else if isLibraryArg(cleanArg) {
				cfg.Libs = append(cfg.Libs, cleanArg)
			} else {
				cfg.ExtraObjs = append(cfg.ExtraObjs, cleanArg)
			}
		}
	}

	if isInitMode {
		if cfg.InputFile == "" {
			cfg.InputFile = "main.cmm"
		}
		os.Exit(initProject(cfg, flags.force))
	}

	if cfg.InputFile == "" {
		if isBuildMode || isCleanMode {
			if confPath == "" {
				fail("no %s found in the current directory and no input .cmm file given on the command line", confFileName)
			}
			fail("%s does not define an 'input' file", confPath)
		}
		fmt.Fprintln(os.Stderr, "error: no input file given")
		fmt.Fprintln(os.Stderr, "usage: gmm [flags] <input.cmm>")
		fmt.Fprintln(os.Stderr, "try 'gmm -h' for more information")
		os.Exit(1)
	}

	if !isCleanMode && !fileExists(cfg.InputFile) {
		fail("input file not found: %s", cfg.InputFile)
	}
	for _, obj := range cfg.ExtraObjs {
		if !isCleanMode && !fileExists(obj) {
			fail("extra object or library not found: %s", obj)
		}
	}
	if !strings.EqualFold(filepath.Ext(cfg.InputFile), ".cmm") {
		fmt.Fprintf(os.Stderr, "warning: %s does not use the .cmm extension\n", cfg.InputFile)
	}

	if err := checkTargetSupported(cfg.TargetType); err != nil && !isCleanMode {
		fail("%v", err)
	}

	inputDir := filepath.Dir(cfg.InputFile)
	baseName := baseNameWithoutExt(cfg.InputFile)

	if !cfg.OutputSet || cfg.Output == "" {
		cfg.Output = DefaultTargetOutput
		cfg.OutputSet = false
	}
	if cfg.IsShared() || cfg.TargetType == "linux" || cfg.TargetType == "exe" {
		if !cfg.OutputSet || cfg.Output == DefaultTargetOutput {
			cfg.Output = resolveOutputName(baseName, cfg.TargetType)
		} else {
			cfg.Output = resolveOutputName(cfg.Output, cfg.TargetType)
		}
	} else if runtime.GOOS == "windows" {
		cfg.Output = resolveOutputName(cfg.Output, cfg.TargetType)
	}

	cfg.CFiles = dedupeStrings(cfg.CFiles)
	cfg.ExtraObjs = dedupeStrings(cfg.ExtraObjs)
	cfg.Libs = dedupeStrings(cfg.Libs)
	cfg.Includes = dedupeStrings(cfg.Includes)
	cfg.Defines = dedupeStrings(cfg.Defines)

	if isCleanMode {
		os.Exit(cleanProject(cfg, inputDir, baseName))
	}

	toolPaths, err := setupEnvironment()
	if err != nil {
		fail("%v", err)
	}
	if toolPaths.CC == "" {
		toolPaths.CC = guessCC(toolPaths.GHC)
	}

	tempDir, err := extractEmbeddedLibs()
	if err != nil {
		fail("extracting embedded libs: %v", err)
	}

	// Ctrl-C must not kill gmm before the temporary directory is removed: the
	// tools it started receive the signal too and stop on their own.
	interrupt := make(chan os.Signal, 1)
	signal.Notify(interrupt, os.Interrupt)

	var code int
	if cfg.Watch {
		code = watchProject(cfg, confPath, toolPaths, tempDir, inputDir, baseName, interrupt)
	} else {
		code = buildAndRun(cfg, toolPaths, tempDir, inputDir, baseName)
	}

	signal.Stop(interrupt)
	os.RemoveAll(tempDir)
	os.Exit(code)
}
