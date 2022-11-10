from runner import Runner
from recorder import Recorder

# CONFIG
TEST_ID_RANGE = [1,27] #【修改】测试样例id范围
TESTCASE_DIR = "./testfile/"
TEST_INPUT_DIR = "./input/"
TEST_OUTPUT_DIR = "./output/"
TEST_INPUT = True # 【修改】是否提供输入
RUN_MARS = False #【修改】是否运行mars
RUN_LLVM = True #【修改】是否运行lli

recorder = Recorder()

for id in range(TEST_ID_RANGE[0], TEST_ID_RANGE[1]+1):
    testPath = TESTCASE_DIR +  "testfile" + str(id) + ".txt"
    outputPath = TEST_OUTPUT_DIR +  "output" + str(id) + ".txt"
    inputPath = None
    if TEST_INPUT:
        inputPath = TEST_INPUT_DIR +  "input" + str(id) + ".txt"
    runner = Runner(testPath=testPath, outputPath=outputPath, inputPath=inputPath, mars=RUN_MARS, llvm=RUN_LLVM)
    runner.run()
    result = runner.check()
    recorder.addResult(id, result)
    if not result:
        break
    print("testing "+str(id) + '/' + str(TEST_ID_RANGE[1]),end='\r')

recorder.writeToLog()       
      
