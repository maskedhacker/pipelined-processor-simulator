import os

os.chdir('simulator/')
retv = os.system('make > /dev/null 2>&1')
if(retv == 0):
	print('makefile success')
else:
	print('makefile ERROR')
	exit()


for file in os.listdir('../student_valid_testcase'):
	file_path = '../student_valid_testcase/'+file
	if(os.path.exists(file_path+'/iimage.bin') & os.path.exists(file_path+'/dimage.bin')):
		os.system('cp ' + file_path+'/iimage.bin iimage.bin')
		os.system('cp ' + file_path+'/dimage.bin dimage.bin')
		os.system('./pipeline > /dev/null 2>&1')
		retv = os.system('diff snapshot.rpt '+file_path+'/snapshot.rpt > /dev/null 2>&1')
		retv |= os.system('diff error_dump.rpt '+file_path+'/error_dump.rpt > /dev/null 2>&1')
		if(retv == 0):
			print('test case '+file + ': pass')
		else:
			print('test case '+file + ': fail')


os.system('make clean > /dev/null 2>&1')
		
