rem #���¶��� image ���� cfg  ���� dump 崻���־

rem IMAGE:
rem #����ĸ�ʽ�����ִ�Сд
rem #����ָ�� 0�ļ��ϴ������� 1�ļ��ϴ�  2�ָ����� 3������� 4���� 5��ȡ�汾��
rem #�������ڵ�CPU_X����·��
rem #�۵��� ��ѡ���� ��δ�������ֶ����� (rack-1)*16 + slot = moudle - 3

rem CFG:
rem # ��������
rem # �۵��� ֻ�ܰ���mvcu ��ѡ���� ��δ�������ֶ����� (rack-1)*16 + slot = moudle - 3

rem DUMP:
rem # ��ӦCPU ֻ��Ϊ A B AB
rem # �۵��� ��ѡ���� ��δ�������ֶ����� (rack-1)*16 + slot = moudle - 3

::python3 light_update.py image S19 0 . 
::python3 light_update.py image S19 0 . 5
::python3 light_update.py cfg CSZ_ECID_VOOB_6diff
python3 light_update.py dump AB
pause