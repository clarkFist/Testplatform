pause
rem #����ĸ�ʽ�����ִ�Сд
rem #����ָ�� 0�ļ��ϴ������� 1�ļ��ϴ�  2�ָ����� 3������� 4���� 5��ȡ�汾��
rem #�������ڵ�CPU_X����·��
rem #�۵��� ��ѡ���� ��δ�������ֶ����� (rack-1)*16 + slot = moudle - 3
start python light_update.py S19 0 r32s32 13-14,18-30,32,34,35
start python light_update.py S19 0 r64s64 11,12
start python light_update.py S19 0 r128s32 3-6
start python light_update.py S19 0 r32s128 7-10
