package main

import (
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

const ghcWinPath = `C:/tools/ghc-9.8.2`

func main() {
	output := flag.String("o", "a.out", "Output executable name")
	libs := flag.String("libs", "", "Additional system libs (comma separated)")
	keepObj := flag.Bool("keep-obj", false, "Keep intermediate .o file")
	version := flag.Bool("v", false, "Print version information")
	flag.Parse()

	if *version {
		fmt.Println("gmm version 0.1.0")
		return
	}

	if flag.NArg() < 1 {
		fmt.Println("Usage: gmm [flags] <input.cmm> [extra_files.o/a...]")
		os.Exit(1)
	}

	setupEnvironment()

	inputFile := flag.Arg(0)
	extraFiles := flag.Args()[1:]

	tempDir, err := extractEmbeddedLibs()
	if err != nil {
		fmt.Fprintf(os.Stderr, "error: %v\n", err)
		os.Exit(1)
	}
	defer os.RemoveAll(tempDir)

	mathObj := ""
	content, _ := os.ReadFile(inputFile)
	if strings.Contains(string(content), "#include \"cmmath.h\"") {
		mathObj = filepath.Join(tempDir, "cmmath.o")
		if err := compileMathLib(tempDir, mathObj); err != nil {
			fmt.Fprintf(os.Stderr, "error building cmmath: %v\n", err)
			os.Exit(1)
		}
	}

	objFile := strings.TrimSuffix(filepath.Base(inputFile), filepath.Ext(inputFile)) + ".o"
	if err := compileWithGHC(inputFile, objFile, tempDir); err != nil {
		os.Exit(1)
	}

	if err := linkWithCC(objFile, mathObj, extraFiles, *output, *libs); err != nil {
		os.Exit(1)
	}

	if !*keepObj {
		os.Remove(objFile)
		if mathObj != "" {
			os.Remove(mathObj)
		}
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

func compileWithGHC(input, output, includePath string) error {
	ghc, _ := getPaths()
	cmd := exec.Command(ghc, "-c", "-no-hs-main", "-I"+includePath, input, "-o", output)
	cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	return cmd.Run()
}

func linkWithCC(cmmObj, mathObj string, extraFiles []string, output, libs string) error {
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

	if libs != "" {
		for _, l := range strings.Split(libs, ",") {
			args = append(args, "-l"+l)
		}
	}

	cmd := exec.Command(cc, args...)
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
