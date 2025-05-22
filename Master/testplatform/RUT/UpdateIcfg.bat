@rem CFG:
@rem # 配置名称
@rem # 槽道号 只能包含mvcu 可选参数 若未给出则手动输入 (rack-1)*16 + slot = moudle - 3

set cfg1=CSZ_MVCU1344-FSFB2-0722
set cfg2=CSZ_ECID_VOOB_6diff
set cfg3=CSZ_MVCU1344_RSSP1_28SVCU_6AUTO3_3
set cfg4=VIOP_BP3_3
python light_update.py cfg %cfg4%
pause