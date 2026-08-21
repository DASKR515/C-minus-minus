package main

import (
	"bufio"
	"embed"
	"flag"
	"fmt"
	"io/fs"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strings"
)

//go:embed libgmm/*
var libFS embed.FS

const (
	ghcWinPath = `C:/tools/ghc-9.8.2`
	Author     = "DASKR"
	Repository = "https://github.com/DASKR515/C-minus-minus"
	Version    = "3.0"
)

type Config struct {
	InputFile  string
	Output     string
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

FLAGS:
  -o <file>        Specify output binary name (default: "a.out" or specified in conf.hmm).
  -b               Build project using configuration from 'conf.hmm'.
  -bc              Build project and clean ONLY object files dynamically created by gmm in this run.
  -r, -run         Run the output executable (uses specified target or binary from conf.hmm).
  -br              Build project using conf.hmm and execute the binary immediately upon success.
  -libs <libs>     Comma-separated list of additional system libraries or paths to link with.
  -keep-obj        Keep intermediate object files (.o) after linking instead of deleting them.
  -llvm            Use GHC LLVM backend (-fllvm) during C-- compilation.
  -rts             Use GHC directly as the linker driver instead of system C compiler (CC).
  -v               Display version and developer information.
  -h, -help        Display this help message and exit.

CONF.HMM CONFIGURATION BLOCKS:
  function "main"    -> Primary .cmm file path.
  function "out"     -> Target output executable name.
  function "C"       -> C source files to compile and link (.c).
  function "lib"     -> System dynamic/static libraries (.so/.a) or include flags (-I/path).
  function "obj"     -> Pre-compiled object files (.o/.a).
  function "defines" -> Preprocessor macro definitions.
  function "flags"   -> Build flags (-llvm, -rts, -keep-obj).

EXAMPLES:
  gmm main.cmm
  gmm -bc
  gmm -br
  gmm -r build/bin/app
`, Version, Author, Repository)

	fmt.Print(helpText)
}

func executeBinary(binPath string) {
	cleanPath := filepath.Clean(binPath)
	if runtime.GOOS == "windows" && !strings.HasSuffix(strings.ToLower(cleanPath), ".exe") {
		if _, err := os.Stat(cleanPath + ".exe"); err == nil {
			cleanPath += ".exe"
		}
	} else if !strings.Contains(cleanPath, string(os.PathSeparator)) && !strings.HasPrefix(cleanPath, ".") {
		cleanPath = "." + string(os.PathSeparator) + cleanPath
	}

	cmd := exec.Command(cleanPath)
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Stdin = os.Stdin

	if err := cmd.Run(); err != nil {
		fmt.Fprintf(os.Stderr, "execution error: %v\n", err)
		os.Exit(1)
	}
}

func main() {
	output := flag.String("o", "a.out", "Output executable name")
	libs := flag.String("libs", "", "Additional system libs (comma separated)")
	keepObj := flag.Bool("keep-obj", false, "Keep intermediate .o file")
	version := flag.Bool("v", false, "Print version information")
	useLLVM := flag.Bool("llvm", false, "Use LLVM backend for GHC (-fllvm)")
	useRTS := flag.Bool("rts", false, "Use GHC for linking directly instead of CC driver")
	buildCmd := flag.Bool("b", false, "Build project using conf.hmm config file")
	buildCleanCmd := flag.Bool("bc", false, "Build project using conf.hmm and clean newly created obj files")
	runCmd := flag.Bool("r", false, "Run executable")
	runCmdLong := flag.Bool("run", false, "Run executable")
	buildRunCmd := flag.Bool("br", false, "Build and run project")
	helpLong := flag.Bool("help", false, "Print help information")
	helpShort := flag.Bool("h", false, "Print help information")

	flag.Usage = printHelp
	flag.Parse()

	if *helpLong || *helpShort {
		printHelp()
		return
	}

	if *version {
		fmt.Printf("gmm version %s\nDeveloper: %s\nGitHub: %s\n", Version, Author, Repository)
		return
	}

	args := flag.Args()

	isBuildMode := *buildCmd || *buildCleanCmd || *buildRunCmd
	isOnlyRunMode := *runCmd || *runCmdLong

	if len(args) > 0 {
		cmdArg := args[0]
		switch cmdArg {
		case "build", "b":
			isBuildMode = true
			args = args[1:]
		case "bc":
			*buildCleanCmd = true
			isBuildMode = true
			args = args[1:]
		case "run", "r":
			isOnlyRunMode = true
			args = args[1:]
		case "br":
			*buildRunCmd = true
			isBuildMode = true
			args = args[1:]
		}
	}

	cfg := Config{
		Output:     *output,
		KeepObj:    *keepObj,
		UseLLVM:    *useLLVM,
		UseRTS:     *useRTS,
		CleanBuild: *buildCleanCmd,
		RunAfter:   *buildRunCmd,
	}

	if *libs != "" {
		cfg.Libs = append(cfg.Libs, strings.Split(*libs, ",")...)
	}

	parseHMMFile("conf.hmm", &cfg)

	if *output != "a.out" {
		cfg.Output = *output
	}

	if isOnlyRunMode && !isBuildMode {
		targetBin := cfg.Output
		if len(args) > 0 {
			targetBin = args[0]
		}
		executeBinary(targetBin)
		return
	}

	if len(args) > 0 {
		cfg.InputFile = args[0]
		for _, arg := range args[1:] {
			cleanArg := filepath.Clean(arg)
			if strings.HasSuffix(cleanArg, ".so") || strings.HasSuffix(cleanArg, ".a") {
				cfg.Libs = append(cfg.Libs, cleanArg)
			} else {
				cfg.ExtraObjs = append(cfg.ExtraObjs, cleanArg)
			}
		}
	}

	if cfg.InputFile == "" {
		if isBuildMode {
			fmt.Println("error: No input .cmm file specified in conf.hmm or CLI.")
		} else {
			printHelp()
		}
		os.Exit(1)
	}

	setupEnvironment()

	tempDir, err := extractEmbeddedLibs()
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
	defer os.RemoveAll(tempDir)

	var createdObjs []string

	compiledCObjs, err := compileCFiles(cfg.CFiles, tempDir, cfg.Includes, cfg.Defines)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error compiling C files: %v\n", err)
		os.Exit(1)
	}

	createdObjs = append(createdObjs, compiledCObjs...)
	cfg.ExtraObjs = append(cfg.ExtraObjs, compiledCObjs...)

	mathObj := ""
	content, _ := os.ReadFile(cfg.InputFile)
	if strings.Contains(string(content), "#include \"cmmath.h\"") {
		mathObj = filepath.Join(tempDir, "cmmath.o")
		if err := compileMathLib(tempDir, mathObj); err != nil {
			fmt.Fprintf(os.Stderr, "error building cmmath: %v\n", err)
			os.Exit(1)
		}
		createdObjs = append(createdObjs, mathObj)
	}

	inputDir := filepath.Dir(cfg.InputFile)
	baseName := strings.TrimSuffix(filepath.Base(cfg.InputFile), filepath.Ext(cfg.InputFile))
	mainObjFile := filepath.Join(inputDir, baseName+".o")

	if err := compileWithGHC(cfg.InputFile, mainObjFile, tempDir, cfg); err != nil {
		os.Exit(1)
	}

	createdObjs = append(createdObjs, mainObjFile)

	outDir := filepath.Dir(cfg.Output)
	if outDir != "." && outDir != "" {
		if err := os.MkdirAll(outDir, 0755); err != nil {
			fmt.Fprintf(os.Stderr, "error creating output directory: %v\n", err)
			os.Exit(1)
		}
	}

	if cfg.UseRTS {
		if err := linkWithGHC(mainObjFile, mathObj, cfg); err != nil {
			os.Exit(1)
		}
	} else {
		if err := linkWithCC(mainObjFile, mathObj, cfg.ExtraObjs, cfg.Output, cfg.Libs); err != nil {
			os.Exit(1)
		}
	}

	if !cfg.KeepObj || cfg.CleanBuild {
		for _, obj := range createdObjs {
			os.Remove(obj)
		}
	}

	if cfg.RunAfter {
		executeBinary(cfg.Output)
	}
}

func getPaths() (ghc, cc string) {
	if runtime.GOOS == "windows" {
		return filepath.Join(ghcWinPath, "bin", "ghc.exe"), filepath.Join(ghcWinPath, "mingw", "bin", "clang.exe")
	}
	return "ghc", "cc"
}

func setupEnvironment() {
	if runtime.GOOS == "windows" {
		if _, err := os.Stat(ghcWinPath); os.IsNotExist(err) {
			installGHCViaChoco()
		}
	}
}

func installGHCViaChoco() {
	fmt.Println("GHC not found. Installing via Chocolatey...")
	chocoCmd := "Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1')); choco install ghc --version 9.8.2 -y"
	exec.Command("powershell", "-Command", chocoCmd).Run()
	fmt.Println("Installation finished. Please restart your terminal.")
	os.Exit(0)
}

func compileMathLib(tempDir, output string) error {
	_, cc := getPaths()
	src := filepath.Join(tempDir, "cmmath.c")
	cmd := exec.Command(cc, "-c", src, "-o", output)
	cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	return cmd.Run()
}

func compileCFiles(cFiles []string, tempDir string, extraIncludes []string, defines []string) ([]string, error) {
	_, cc := getPaths()
	var objs []string

	for _, cFile := range cFiles {
		cleanPath := filepath.Clean(cFile)
		dir := filepath.Dir(cleanPath)
		baseName := strings.TrimSuffix(filepath.Base(cleanPath), filepath.Ext(cleanPath))

		if dir != "." && dir != "" {
			if err := os.MkdirAll(dir, 0755); err != nil {
				return nil, err
			}
		}

		objName := filepath.Join(dir, baseName+"_c.o")
		args := []string{"-c", cleanPath, "-I" + tempDir, "-o", objName}

		for _, inc := range extraIncludes {
			args = append(args, "-I"+inc)
		}
		for _, def := range defines {
			args = append(args, "-D"+def)
		}

		cmd := exec.Command(cc, args...)
		cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
		if err := cmd.Run(); err != nil {
			return nil, err
		}
		objs = append(objs, objName)
	}

	return objs, nil
}

func compileWithGHC(input, output, includePath string, cfg Config) error {
	ghc, _ := getPaths()
	args := []string{"-c", "-no-hs-main"}

	if cfg.UseLLVM {
		args = append(args, "-fllvm")
	}

	args = append(args, "-I"+includePath)

	for _, inc := range cfg.Includes {
		args = append(args, "-I"+inc)
	}
	for _, def := range cfg.Defines {
		args = append(args, "-optP-D"+def)
	}

	args = append(args, input, "-o", output)

	cmd := exec.Command(ghc, args...)
	cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	return cmd.Run()
}

func linkWithCC(cmmObj, mathObj string, extraFiles []string, output string, libs []string) error {
	_, cc := getPaths()
	args := []string{cmmObj}
	if mathObj != "" {
		args = append(args, mathObj)
	}
	args = append(args, extraFiles...)

	if runtime.GOOS == "windows" {
		args = append(args, "-o", output, "-g")
	} else {
		args = append(args, "-o", output, "-no-pie", "-g", "-lm", "-lc")
	}

	for _, l := range libs {
		l = strings.TrimSpace(l)
		if l == "" {
			continue
		}
		if strings.HasPrefix(l, "-") || strings.Contains(l, "/") || strings.Contains(l, "\\") || strings.HasSuffix(l, ".so") || strings.HasSuffix(l, ".a") {
			args = append(args, l)
		} else {
			args = append(args, "-l"+l)
		}
	}

	cmd := exec.Command(cc, args...)
	cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	return cmd.Run()
}

func linkWithGHC(cmmObj, mathObj string, cfg Config) error {
	ghc, _ := getPaths()
	args := []string{"-no-hs-main", cmmObj}
	if mathObj != "" {
		args = append(args, mathObj)
	}
	args = append(args, cfg.ExtraObjs...)

	if cfg.UseLLVM {
		args = append(args, "-fllvm")
	}

	args = append(args, "-o", cfg.Output)

	for _, l := range cfg.Libs {
		l = strings.TrimSpace(l)
		if l == "" {
			continue
		}
		if strings.HasPrefix(l, "-") || strings.Contains(l, "/") || strings.Contains(l, "\\") || strings.HasSuffix(l, ".so") || strings.HasSuffix(l, ".a") {
			args = append(args, l)
		} else {
			args = append(args, "-l"+l)
		}
	}

	cmd := exec.Command(ghc, args...)
	cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	return cmd.Run()
}

func extractEmbeddedLibs() (string, error) {
	tempDir, err := os.MkdirTemp("", "gmm-")
	if err != nil {
		return "", err
	}
	err = fs.WalkDir(libFS, "libgmm", func(path string, d fs.DirEntry, err error) error {
		if err != nil || d.IsDir() {
			return err
		}
		content, _ := libFS.ReadFile(path)
		return os.WriteFile(filepath.Join(tempDir, filepath.Base(path)), content, 0644)
	})
	return tempDir, err
}

func parseHMMFile(filename string, cfg *Config) {
	file, err := os.Open(filename)
	if err != nil {
		return
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	currentSection := ""

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" || strings.HasPrefix(line, "#") || strings.HasPrefix(line, "//") {
			continue
		}

		if strings.HasPrefix(line, "function ") {
			parts := strings.Fields(line)
			if len(parts) >= 2 {
				currentSection = strings.Trim(parts[1], "\"")
			}
			continue
		}

		if line == "end" {
			currentSection = ""
			continue
		}

		switch currentSection {
		case "main":
			cfg.InputFile = filepath.Clean(line)
		case "out":
			cfg.Output = line
		case "lib":
			if strings.HasPrefix(line, "-I") {
				cfg.Includes = append(cfg.Includes, strings.TrimPrefix(line, "-I"))
			} else {
				cfg.Libs = append(cfg.Libs, line)
			}
		case "C":
			cfg.CFiles = append(cfg.CFiles, filepath.Clean(line))
		case "obj":
			cfg.ExtraObjs = append(cfg.ExtraObjs, filepath.Clean(line))
		case "defines":
			cfg.Defines = append(cfg.Defines, strings.TrimPrefix(line, "-D"))
		case "flags":
			if line == "-llvm" {
				cfg.UseLLVM = true
			} else if line == "-rts" {
				cfg.UseRTS = true
			} else if line == "-keep-obj" {
				cfg.KeepObj = true
			}
		}
	}
}
