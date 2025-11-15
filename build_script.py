import os
import subprocess
import shutil
import sys

def run_command(command, cwd=None):
    print("\n" + "="*50)
    print(f"Running command: {command}")
    
    result = subprocess.run(command, shell=True, cwd=cwd, text=True)
    
    # if error, print it
    if result.returncode != 0:
        print("Command failed with return code:", result.returncode)
        print("="*50 + "\n")
        raise RuntimeError("Command failed")

def main():
    # directory paths
    project_root = os.path.abspath(os.path.dirname(__file__))
    build_dir = os.path.join(project_root, 'build')
    bin_dir = os.path.join(project_root, 'bin')

    # linux commands
    cmake_config_command = 'cmake .. -DCMAKE_BUILD_TYPE=Debug'
    cmake_build_command = 'make'
    run_command_exe = './src'
    unit_test_command = 'ctest --output-on-failure --verbose'

    # make build/build dir
    os.makedirs(build_dir, exist_ok=True)
    os.chdir(build_dir)

    def run_exe():
        os.chdir(bin_dir)
        run_command(run_command_exe)

    def run_tests():
        run_command(unit_test_command, cwd=build_dir)

    def compile():
        run_command(cmake_build_command)

    def config_cmake():
        # if build/build exists, delete it
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)
        os.makedirs(build_dir, exist_ok=True)
        os.chdir(build_dir)
        
        # execute build commands
        run_command(cmake_config_command)

    def run_all():
        config_cmake()
        compile()
        run_tests()
        run_exe()

    print("Choose a build option: ")
    print("1. Configure Cmake")
    print("2. Compile Code")
    print("3. Run Unit Tests")
    print("4. Run executable")
    print("5. Entire Process")

    while True:
        choice = input("Enter your choice (1-5) : ")
        if choice == '1': 
            config_cmake()
            break
        elif choice == '2':
            compile()
            break
        elif choice == '3':
            run_tests()
            break
        elif choice == '4':
            run_exe()
            break
        elif choice == '5':
            run_all()
            break
        else:
            print("Invalid choice. Try again")


if __name__ == '__main__':
    main()