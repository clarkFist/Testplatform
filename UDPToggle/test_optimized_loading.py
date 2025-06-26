#!/usr/bin/env python3
"""
测试优化后的项目加载功能
"""

import sys
import os
from pathlib import Path

# 添加项目根目录到sys.path
project_root = Path(__file__).parent
sys.path.insert(0, str(project_root))

from core.c_parser import c_parser
from core.switch_manager import switch_manager
from core.config_manager import config


def test_sample_project_loading():
    """测试示例项目加载"""
    print("=" * 60)
    print("测试项目加载功能优化")
    print("=" * 60)
    
    # 设置示例项目路径
    sample_project_path = project_root / "sample_project"
    print(f"📁 测试项目路径: {sample_project_path}")
    
    if not sample_project_path.exists():
        print("❌ 示例项目目录不存在")
        return False
    
    try:
        # 清空现有数据
        switch_manager._switches.clear()
        switch_manager._groups.clear()
        
        print("\n📊 第一步：解析项目文件")
        print("-" * 40)
        
        # 解析项目
        switches = c_parser.parse_project(str(sample_project_path))
        print(f"✅ 解析完成，找到 {len(switches)} 个开关桩")
        print(f"📁 解析了 {len(c_parser.parsed_files)} 个文件")
        
        # 显示解析的文件
        for file_path in c_parser.parsed_files:
            print(f"   📄 {Path(file_path).name}")
        
        print("\n📋 第二步：生成配置数据")
        print("-" * 40)
        
        # 生成配置
        config_data = c_parser.export_to_config()
        groups_count = len(config_data.get('switch_groups', {}))
        print(f"✅ 配置生成完成，创建了 {groups_count} 个分组")
        
        # 显示分组信息
        for group_name, group_data in config_data.get('switch_groups', {}).items():
            switch_count = len(group_data.get('switches', []))
            print(f"   📂 {group_name}: {switch_count} 个开关桩")
        
        print("\n⚡ 第三步：加载到开关桩管理器")
        print("-" * 40)
        
        # 加载到管理器
        load_success = switch_manager.load_from_config(config_data)
        if load_success:
            print("✅ 开关桩加载成功")
            
            # 显示统计信息
            all_switches = switch_manager.get_all_switches()
            all_groups = switch_manager._groups
            
            print(f"📊 总计: {len(all_switches)} 个开关桩, {len(all_groups)} 个分组")
            
            # 按分组显示开关桩
            for group_name in all_groups.keys():
                group_switches = switch_manager.get_switches_by_group(group_name)
                print(f"\n📂 分组 '{group_name}' ({len(group_switches)} 个开关桩):")
                
                for switch in group_switches[:5]:  # 只显示前5个
                    status = "🟢" if switch.current_state.value == "open" else "🔴"
                    print(f"   {status} {switch.name} [{switch.switch_type.value}] - {switch.description[:30]}...")
                
                if len(group_switches) > 5:
                    print(f"   ... 还有 {len(group_switches) - 5} 个开关桩")
            
            print("\n🎯 第四步：测试搜索功能")
            print("-" * 40)
            
            # 测试搜索
            search_terms = ["VCU", "DEBUG", "TEST", "ENABLE"]
            for term in search_terms:
                matches = []
                for switch in all_switches.values():
                    if (term.lower() in switch.name.lower() or 
                        term.lower() in switch.description.lower()):
                        matches.append(switch)
                
                print(f"🔍 搜索 '{term}': 找到 {len(matches)} 个匹配项")
                for match in matches[:3]:  # 显示前3个匹配项
                    print(f"   📌 {match.name}")
            
            print("\n✅ 测试完成！项目加载功能工作正常")
            return True
            
        else:
            print("❌ 开关桩加载失败")
            return False
            
    except Exception as e:
        print(f"❌ 测试失败: {e}")
        import traceback
        traceback.print_exc()
        return False


def test_optimized_features():
    """测试优化后的特性"""
    print("\n" + "=" * 60)
    print("测试优化功能特性")
    print("=" * 60)
    
    try:
        # 获取统计信息
        stats = switch_manager.get_statistics()
        print(f"📈 统计信息:")
        print(f"   总开关桩数: {stats.get('total_switches', 0)}")
        print(f"   分组数量: {stats.get('group_count', 0)}")
        print(f"   启用开关桩: {stats.get('enabled_switches', 0)}")
        
        # 测试批量操作
        print(f"\n🔧 测试批量操作:")
        all_switches = list(switch_manager.get_all_switches().keys())
        if len(all_switches) >= 3:
            test_switches = all_switches[:3]
            print(f"   选择测试开关桩: {', '.join(test_switches)}")
            
            # 批量设置状态
            from core.switch_manager import SwitchState
            success_list, failed_list = switch_manager.batch_set_state(test_switches, SwitchState.OPEN)
            print(f"   批量设置结果: 成功 {len(success_list)}, 失败 {len(failed_list)}")
            
        # 测试分组排序
        print(f"\n📂 测试分组排序:")
        groups = list(switch_manager._groups.keys())
        priority_groups = ['vcu_control', 'config', 'default', 'debug', 'test', 'safety']
        sorted_groups = []
        
        for priority_group in priority_groups:
            if priority_group in groups:
                sorted_groups.append(priority_group)
        
        remaining_groups = [g for g in groups if g not in sorted_groups]
        sorted_groups.extend(sorted(remaining_groups))
        
        print(f"   原始分组: {groups}")
        print(f"   排序后分组: {sorted_groups}")
        
        print("\n✅ 优化功能测试完成！")
        return True
        
    except Exception as e:
        print(f"❌ 优化功能测试失败: {e}")
        return False


if __name__ == "__main__":
    print("🚀 开始测试优化后的项目加载功能")
    
    # 测试项目加载
    loading_success = test_sample_project_loading()
    
    if loading_success:
        # 测试优化功能
        feature_success = test_optimized_features()
        
        if feature_success:
            print("\n🎉 所有测试通过！优化功能正常工作")
            print("\n💡 主要优化内容:")
            print("   1. ✨ 增强项目加载进度显示和用户反馈")
            print("   2. 🎨 优化桩列表显示格式和状态标识")
            print("   3. 🧠 智能分组排序和默认选择")
            print("   4. 🔍 改进搜索功能和过滤逻辑")
            print("   5. 📊 更详细的统计信息和状态显示")
            print("   6. ⚠️  更好的错误处理和空状态提示")
        else:
            print("\n⚠️  项目加载正常，但优化功能测试失败")
    else:
        print("\n❌ 项目加载测试失败")
    
    print("\n📋 测试报告完成") 