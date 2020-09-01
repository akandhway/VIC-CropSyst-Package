object navigation_frame: Tnavigation_frame
  Left = 0
  Top = 0
  Width = 693
  Height = 281
  TabOrder = 0
  object shellnavigator_current_directory: TStShellNavigator
    Left = 0
    Top = 0
    Width = 693
    Height = 33
    Buttons = [nbBack, nbMoveUp, nbNewFolder, nbList, nbDetails, nbView]
    ListView = shelllistview_current_directory
    LeftOffset = 10
    Style = nsWinXP
    Align = alTop
    BevelOuter = bvNone
    ParentShowHint = False
    ShowHint = True
    TabOrder = 0
    object button_launch_text_edit: TSpeedButton
      Left = 565
      Top = 4
      Width = 23
      Height = 22
      Hint = 'Edits the currently selected file with notepad'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        26040000424D2604000000000000360000002800000012000000120000000100
        180000000000F003000000000000000000000000000000000000CED3D6CED3D6
        CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3
        D6CED3D6CED3D6CED3D6CED3D6CED3D60000CED3D6CED3D6CED3D6CED3D6CED3
        D6CED3D600000000000000000000000000000000000000000000000000000000
        0000CED3D6CED3D60000CED3D6CED3D6CED3D6CED3D6CED3D6000000C6C7C6C6
        C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6848684000000CED3D6
        0000CED3D6CED3D6CED3D6CED3D6CED3D6000000C6C7C6C6C7C6C6C7C6C6C7C6
        C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6000000CED3D60000CED3D6CED3D6
        CED3D6CED3D6CED3D6000000C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7
        C6FFFFFFFFFFFFC6C7C6000000CED3D60000CED3D6CED3D6CED3D6CED3D6CED3
        D6000000C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6C6
        C7C6000000CED3D60000CED3D600000000000000000000000000000000000000
        0000000000000000000000C6C7C6C6C7C6C6C7C6C6C7C6C6C7C6000000CED3D6
        0000CED3D6848600FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        848600000000C6C7C6C6C7C6FFFFFFC6C7C6000000CED3D60000CED3D6000000
        FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00000000C6C7
        C6C6C7C6C6C7C6C6C7C6000000CED3D60000CED3D6CED3D6848600FFFF00FFFF
        00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00848600000000C6C7C6C6C7C6C6
        C7C6000000CED3D60000CED3D6CED3D6000000FFFF00FFFF00FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00000000C6C7C6C6C7C6C6C7C6000000CED3D6
        0000CED3D6CED3D6CED3D6848600FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
        FFFF00FFFF00848600000000C6C7C6C6C7C6000000CED3D60000CED3D6CED3D6
        CED3D6000000FFFF00FFFF00848600848600848600848600848600FFFF00FFFF
        00000000C6C7C6C6C7C6000000CED3D60000CED3D6CED3D6CED3D6CED3D68486
        00FFFF00848600848600848600848600848600848600FFFF00848600000000C6
        C7C6000000CED3D60000CED3D6CED3D6CED3D6CED3D6000000FFFF00FFFF00FF
        FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00000000C6C7C6000000CED3D6
        0000CED3D6CED3D6CED3D6CED3D6CED3D6000000848684848600848600848684
        848600848600848684848600000000000000CED3D6CED3D60000CED3D6CED3D6
        CED3D6CED3D6CED3D6CED3D60000000000000000000000000000000000000000
        00000000000000CED3D6CED3D6CED3D60000CED3D6CED3D6CED3D6CED3D6CED3
        D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CED3D6CE
        D3D6CED3D6CED3D60000}
      ParentShowHint = False
      ShowHint = True
      OnClick = button_launch_text_editClick
    end
    object button_launch_windows_explorer: TSpeedButton
      Left = 592
      Top = 4
      Width = 23
      Height = 22
      Hint = 'Open a Windows explorer window here.'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        66010000424D6601000000000000760000002800000014000000140000000100
        040000000000F000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
        888888880000888887770000070000080000888800000FFF00FFFF0800008888
        08880FFF0BFFFF080000888808880FFF0007FF08000088880BBB05500CC05008
        000088880BBB0000CCC00088000088880888BB8CCC078788000088880888B0CC
        C03887880000888808880CCC00000888000088880330CCC07888888800008870
        0008CC07888888880000870EEEE880788888888800008EEEEEEE077888888888
        00000EEEEEEEE0788888888800000EEEEEEEE0788888888800000EFFEEEEE078
        8888888800008EFFEEEEE77888888888000080EEEEEE08888888888800008880
        00088888888888880000}
      ParentShowHint = False
      ShowHint = True
      OnClick = button_launch_windows_explorerClick
    end
    object button_command_prompt: TSpeedButton
      Left = 619
      Top = 4
      Width = 23
      Height = 22
      Hint = 'Open command prompt window here.'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        F6000000424DF600000000000000760000002800000010000000100000000100
        04000000000080000000120B0000120B00001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
        8888888888888888888877777777777777777000000000000007700000000000
        000770FF000000F0FF077F00F0F00F0000077F0000000F0000077F00F0F00F00
        000770FF0000F000000770000000000000077000000000000007444444444444
        4444CCCCCCCCCC6C6C1C8CCCCCCCCCCCCCC88888888888888888}
      ParentShowHint = False
      ShowHint = True
      OnClick = button_command_promptClick
    end
    object button_restore_window_position: TSpeedButton
      Left = 642
      Top = 4
      Width = 23
      Height = 22
      Hint = 
        'Restore CS explorer window to last saved position and size for t' +
        'his context'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        36040000424D3604000000000000360000002800000010000000100000000100
        2000000000000004000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000007D4D32FF6A3E28FF5E2F17FF5E2F
        17FF5E2F17FF5E2F17FF5E2F17FF5E2F17FF5E2F17FF5E2F17FF000000000000
        000000000000000000000000000000000000AC7558FFFEFBF9FFFCF5F2FFF9EE
        E9FFF7E9E2FFF5E4DAFFF1DACEFFEFD5C7FFEDD0C0FF5E2F17FF000000000000
        000000000000000000000000000000000000B67E61FFFFFBFAFFFEFAF9FFFEF9
        F8FFFDF7F4FFFAF1ECFFF8ECE6FFF6E5DDFFF3DFD3FF804020FF000000000000
        000000000000000000000000000000000000C68B6DFFFFFCFBFFFFFCFBFFFEFB
        FAFFFEF9F6FFFDF8F6FFFCF6F2FFFBF3EFFFF8EAE3FF8D4C2CFF867063FF7965
        5BFF725C53FF806C63FF93827BFFA59690FFE9AA8BFFEAA98AFFE7A483FFE49C
        79FFE1946EFFDD8B62FFD98156FFD5794AFFD1703FFFCE6836F9AA9283FFFCFB
        FAFFFAF7F6FFF7F1F0FFF6F1EDFFF4EEEBFFECAD8FFFFFFFFFFFFEC1A0FFFCBA
        96FFFAB08AFFF8A67DFFF59E71FFF39767FFF59767FFD36F3DF7B2998BFFFDFB
        FAFFFCFAFAFFFCFAFAFFFBF9F8FFF8F4F2FFEAAC8EFFEAA98AFFE7A383FFE49C
        79FFE0946EFFDD8B62FFD98256FFD6784AFFD16F3FFFCF6937F7C0A597FFFDFC
        FBFFFDFCFBFFFCFBFAFFFCFAF9FFFBF9F8FFFAF8F6FFF9F5F4FFF7F1EEFF9C82
        76FF000000000000000000000000000000000000000000000000DBBDAFFBDBBD
        AFFFD9BAABFFD5B4A4FFD0AC9BFFCCA693FFC8A08FFFC49C88FFC29984FFBC90
        7DFF786E6AFF756A66FF746965FF746965FF746965FF00000000DDBFB1F9FFFF
        FFFFECD0C1FFE9CBBAFFE5C4B2FFE2BDAAFFDEB7A2FFDBB09BFFDCB29DFFC195
        7FFFEFECEAFFEBE6E4FFE8E3E0FFE5DFDCFF746965FF00000000DBBDAFF7DBBD
        AFFFD9BAABFFD5B4A4FFCFAC9BFFCCA693FFC79F8CFFC39883FFBF917CFFBE90
        7AFFF7F5F4FFF4F1F0FFF1EDECFFEDE9E6FF857872FF00000000000000000000
        0000000000000000000000000000BDB1AAFFFDFCFCFFFDFCFCFFFCFBFBFFFBFA
        FAFFFBFAFAFFF9F8F7FFF8F7F6FFF4F1F0FF8F827CFF00000000000000000000
        0000000000000000000000000000D5C6C1FBD5C6C1FFD1C4BDFFCDBEB7FFCABA
        B2FFC4B2AAFFBFADA5FFB9A79DFFB5A197FFB29C92F900000000000000000000
        0000000000000000000000000000D6C9C3F9FFFFFFFFE4D7D1FFE0D3CBFFDCCD
        C5FFD8C7BFFFD4C2B8FFCFBCB3FFD1BDB4FFB4A096F700000000000000000000
        0000000000000000000000000000D5C6C1F7D5C6C1FFD1C3BDFFCDBEB7FFC9B9
        B1FFC4B2AAFFBFAEA5FFBAA79EFFB5A197FFB39D93F700000000}
      ParentShowHint = False
      ShowHint = True
      OnClick = button_restore_window_positionClick
    end
    object button_set_window_position: TSpeedButton
      Left = 665
      Top = 4
      Width = 23
      Height = 22
      Hint = 'Save CS explorer window position and size for this context'
      Anchors = [akTop, akRight]
      Glyph.Data = {
        66010000424D6601000000000000760000002800000016000000140000000100
        040000000000F000000000000000000000001000000000000000000000000000
        8000008000000080800080000000800080008080000080808000C0C0C0000000
        FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00888888888888
        8888888888FF8888888888888888888888FF8888800000000000000088FF8888
        808888888888888088FF8888808888888888888088FB88888088888888888880
        88FA8111108888888888888088FF8188808888888888888088FF818880000000
        0000000088BF818880F000000000000088AF8188800000000000000088FB8188
        888888888188888888FA8111111111111111118888BF81F11111111111888188
        88AF8111111111111188818888FB8888188888888888818888FA888818888888
        8888818888BF8888111111111111118888AF88881F1111111111118888FF8888
        111111111111118888FF}
      ParentShowHint = False
      ShowHint = True
      OnClick = button_set_window_positionClick
    end
  end
  object pagecontrol_navigation: TPageControl
    Left = 0
    Top = 55
    Width = 693
    Height = 226
    ActivePage = tabsheet_expore
    Align = alClient
    MultiLine = True
    TabOrder = 1
    TabPosition = tpRight
    object tabsheet_expore: TTabSheet
      Caption = 'Explorer'
      object Splitter1: TSplitter
        Left = 300
        Top = 0
        Width = 3
        Height = 216
        Cursor = crHSplit
      end
      object shelltreeview_current_directory: TStShellTreeView
        Left = 0
        Top = 0
        Width = 300
        Height = 216
        CompressedColor = clBlue
        ExpandInterval = 2000
        Filtered = False
        ListView = shelllistview_current_directory
        Options = [toAllowRename, toAllowDrag, toAllowDrop, toExpandTopNode, toShellMenu]
        SpecialRootFolder = sfDesktop
        SpecialStartInFolder = sfNone
        OnFolderSelected = shelltreeview_current_directoryFolderSelected
        Align = alLeft
        Indent = 19
        ParentColor = False
        ShowRoot = False
        TabOrder = 0
      end
      object shelllistview_current_directory: TStShellListView
        Left = 303
        Top = 0
        Width = 363
        Height = 216
        CompressedColor = clBlue
        Filtered = False
        OpenDialogMode = True
        Optimization = otEnumerate
        Options = [loAllowRename, loAllowDrag, loAllowDrop, loExtendedMenu, loShellMenu, loSortTypeByExt]
        SpecialRootFolder = sfNone
        TreeView = shelltreeview_current_directory
        ViewStyle = vsReport
        OnItemDblClick = shelllistview_current_directoryItemDblClick
        OnItemSelected = shelllistview_current_directoryItemSelected
        Align = alClient
        MultiSelect = True
        ReadOnly = False
        TabOrder = 1
        OnChange = shelllistview_current_directoryChange
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Recent'
      ImageIndex = 1
      object recent_scenario_shelllistview: TStShellListView
        Left = 0
        Top = 0
        Width = 666
        Height = 216
        CompressedColor = clBlue
        Filtered = True
        FileFilter = '*.csn;*.csn.lnk;*.CS_scenario;*.CS_scenario.lnk'
        OpenDialogMode = True
        Optimization = otEnumerate
        Options = [loAllowRename, loAllowDrag, loAllowDrop, loShellMenu, loShowHidden]
        SpecialRootFolder = sfRecentFiles
        ViewStyle = vsReport
        OnItemDblClick = recent_scenario_shelllistviewItemDblClick
        Align = alClient
        MultiSelect = True
        ReadOnly = False
        TabOrder = 0
      end
    end
  end
  object panel_location: TPanel
    Left = 0
    Top = 33
    Width = 693
    Height = 22
    Align = alTop
    Alignment = taLeftJustify
    BevelOuter = bvNone
    Caption = 'Current:'
    TabOrder = 2
    object edit_current_folder_path: TEdit
      Left = 40
      Top = 1
      Width = 652
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      Color = cl3DLight
      TabOrder = 0
      Text = 'path'
    end
  end
  object shortcutter: TStShortcut
    IconIndex = 0
    SpecialFolder = sfDesktop
    Left = 244
    Top = 129
  end
end
