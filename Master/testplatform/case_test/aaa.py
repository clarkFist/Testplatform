import caseFunc
svcus = caseFunc.Config().active_svcu()
svcus = '2.3.3-8'
svcus = caseFunc.VCU.inputsplit(svcus)
print(svcus)