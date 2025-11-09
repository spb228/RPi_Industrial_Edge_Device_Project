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
    project_root = os.path.abspath(os.path.dirname(__file__))
    build_dir = os.path.join(project_root, 'build')
    bin_dir = os.path.join(project_root, 'bin')

    # if build/build exists, delete it
    if os.path.exists(build_dir):
        shutil.rmtree(build_dir)

    # make build/build dir
    os.makedirs(build_dir, exist_ok=True)
    os.chdir(build_dir)

    # linux commands
    cmake_config_command = 'cmake .. -DCMAKE_BUILD_TYPE=Debug'
    cmake_build_command = 'make'
    run_command_exe = './src'

    # execute build commands
    run_command(cmake_config_command)
    run_command(cmake_build_command)
    os.chdir(bin_dir)
    run_command(run_command_exe)

if __name__ == '__main__':
    main()