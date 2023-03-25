import os
import argparse
import shutil

# global variables, move them to environment or something if you care
src = "\\src"
build = "\\.build"
target = "main.exe"

#compileFlags = "-Wconversion -Werror -std=c99 -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes -Wall -Wno-missing-braces"
compileFlags = ""
linkedLibs = "-lopengl32 -lglfw3 -lgdi32"

ignoreCompilerFail = False

rootdir = os.getcwd()

def makeDirIfNotExisting(path: str):
	if (not os.path.isdir(path)):
		os.makedirs(path)


args = argparse.ArgumentParser()
args.add_argument("-re", "--rebuild", help="rebuilds entire project", action="store_true")
args.add_argument("--ignore-comp-fail", help="runs old target if there were compiler errors", action="store_true")

parsed = args.parse_args()

if (parsed.rebuild):
	# god, have i rebuilt everything for no reason too often
	print("Are you sure you want to rebuild everything? Y/N")

	answer = input()

	while (not (answer in ["Y", "N"])):
		answer = input()
	
	if (answer == "Y"):
		shutil.rmtree("." + build)

if (parsed.ignore_comp_fail):
	ignoreCompilerFail = True

# make build directory if it doesn't exist
makeDirIfNotExisting(rootdir + build)

sourceFiles = []

print("searching for source files")
# get all files we have to compile
for p, dirs, files in os.walk(rootdir + src):
	p = p + "\\"
	for file in files:
		if (file.endswith(".c")):
			sourceFiles.append(p.removeprefix(rootdir + src) + file.removesuffix(".c"))

compilationFailed = False

# compiles source file to appropriate directory in .build, creating one if need be
def compileToObject(sourceFile: str):
	print("compiling " + sourceFile.removeprefix("\\"))
	startingDir = os.getcwd()
	
	os.chdir(rootdir + build)

	split = sourceFile[::-1].split("\\", 1)

	# expected input is in the style "[\path]\file". we reverse it because there's no split last, 
	# split it by \ with at most 1 split and thereby get "[hatp\]". we then reverse this again
	# to get \path, and use it to generate a folder and move to it if need be
	if (len(split) == 2 and len(split[-1]) > 0):
		subdir = split[-1][::-1]
		makeDirIfNotExisting(rootdir + build + "\\" + subdir)
		os.chdir(subdir.removeprefix("\\"))

	command = "gcc -c \"" + rootdir + src + sourceFile + ".c\" " + compileFlags
	returnCode = os.system(command)

	os.chdir(startingDir)
	
	return returnCode

def isCompiledOlderThanSource(file: str):
	return os.path.getmtime(rootdir + build + file + ".o") < os.path.getmtime(rootdir + src + file + ".c")

# compiling
print("begun compiling")
compiledAnything = False

for file in sourceFiles:
	shouldCompile = False

	if (not os.path.isfile(rootdir + build + file + ".o") or isCompiledOlderThanSource(file)):
		returnCode = compileToObject(file)

		compiledAnything = compiledAnything or returnCode == 0
		compilationFailed = compilationFailed or returnCode != 0

if compilationFailed and not ignoreCompilerFail:
	print("compilation failed, not running main. use --ignore-comp-fail if you want to run anyway")
	exit(1)

elif compilationFailed:
	print("compilation failed, running main anyway")

else:
	# linking step
	if (compiledAnything or not os.path.isfile(rootdir + "\\" + target)):
		print("building executable")
		linking = ""

		for file in sourceFiles:
			linking += "." + build + file + ".o "

		os.system("gcc -o " + target + " " + linking + " " + linkedLibs + " -Wall")

		print("executable built, running " + target)

	else:
		print("executable up to date, running " + target)

# run the thing
os.system(".\\" + target)