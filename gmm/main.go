package main

import (
        "embed"
        "flag"
        "fmt"
        "io/fs"
        "os"
        "os/exec"
        "path/filepath"
        "strings"
)

//go:embed libgmm/*
var libFS embed.FS

func main() {
        output := flag.String("o", "a.out", "Output executable name")
        libs := flag.String("libs", "", "Additional system libs (comma separated)")
        keepObj := flag.Bool("keep-obj", false, "Keep intermediate .o file")
        flag.Parse()

        if flag.NArg() < 1 {
                fmt.Println("Usage: gmm [flags] <input.cmm> [extra_files.o/a...]")
                os.Exit(1)
        }

        inputFile := flag.Arg(0)
        extraFiles := flag.Args()[1:]

        checkDependencies()

        tempDir, err := extractEmbeddedLibs()
        if err != nil {
                fmt.Fprintf(os.Stderr, "error: failed to extract libs: %v\n", err)
                os.Exit(1)
        }
        defer os.RemoveAll(tempDir)

        
        mathObj := ""
        content, _ := os.ReadFile(inputFile)
        if strings.Contains(string(content), "#include \"cmmath.h\"") {
                fmt.Println("Info: cmmath.h detected, building and linking cmmath.o")
                mathObj = filepath.Join(tempDir, "cmmath.o")
                if err := compileMathLib(tempDir, mathObj); err != nil {
                        fmt.Fprintf(os.Stderr, "error: failed to build cmmath.c: %v\n", err)
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

func compileMathLib(tempDir, output string) error {
        src := filepath.Join(tempDir, "cmmath.c")
        cmd := exec.Command("cc", "-c", src, "-o", output)
        cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
        return cmd.Run()
}

func compileWithGHC(input, output, includePath string) error {
        cmd := exec.Command("ghc", "-c", "-no-hs-main", "-I"+includePath, input, "-o", output)
        cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
        return cmd.Run()
}

func linkWithCC(cmmObj, mathObj string, extraFiles []string, output, libs string) error {
        args := []string{cmmObj}
        if mathObj != "" {
                args = append(args, mathObj)
        }
        args = append(args, extraFiles...)
        args = append(args, "-o", output, "-no-pie", "-g", "-lm", "-lc")

        if libs != "" {
                for _, l := range strings.Split(libs, ",") {
                        args = append(args, "-l"+l)
                }
        }

        cmd := exec.Command("cc", args...)
        cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
        return cmd.Run()
}

func checkDependencies() {
        for _, cmd := range []string{"ghc", "cc"} {
                if _, err := exec.LookPath(cmd); err != nil {
                        fmt.Printf("error: %s not found.\n", cmd)
                        os.Exit(1)
                }
        }
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
