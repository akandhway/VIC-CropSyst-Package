object Irrigation_application_form: TIrrigation_application_form
  Left = 1264
  Top = 398
  Align = alClient
  Caption = 'Irrigation application'
  ClientHeight = 290
  ClientWidth = 508
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object PageControl1: TPageControl
    Left = 0
    Top = 0
    Width = 508
    Height = 290
    ActivePage = TabSheet2
    Align = alClient
    TabOrder = 0
    ExplicitHeight = 291
    object tabsheet_application_conditions: TTabSheet
      Caption = 'Application conditions'
      ExplicitHeight = 263
      object label_app_cond_auto: TLabel
        Left = 0
        Top = 236
        Width = 500
        Height = 26
        Align = alBottom
        AutoSize = False
        Caption = 
          'Application conditions only apply to automatic irrigation or per' +
          'iodic specified events'
        Color = clInfoBk
        ParentColor = False
        WordWrap = True
        ExplicitTop = 212
      end
      object panel_consideration_mode: TPanel
        Left = 0
        Top = 0
        Width = 500
        Height = 50
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object Label2: TLabel
          Left = 185
          Top = 0
          Width = 315
          Height = 50
          Align = alClient
          AutoSize = False
          Caption = 
            'Typically '#39'Soil water depletion'#39' mode is used. Leaf water potent' +
            'ial mode is intended for fruit orchard/vineyard.'
          Color = clInfoBk
          ParentColor = False
          WordWrap = True
        end
        object radiogroup_consideration: TRNAutoRadioGroup
          Left = 0
          Top = 0
          Width = 185
          Height = 50
          Align = alLeft
          Caption = 'Consideration mode'
          Items.Strings = (
            'Soil water depletion'
            'Leaf water potential')
          TabOrder = 0
          OnClick = radiogroup_considerationClick
        end
      end
      object groupbox_depletion_constraints: TGroupBox
        Left = 0
        Top = 77
        Width = 500
        Height = 151
        Align = alTop
        Caption = 'Depletion constraints'
        TabOrder = 1
        object edit_max_allowable_depletion: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 18
          Width = 490
          Height = 22
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Maximum allowable depletion'
          TabOrder = 0
        end
        object edit_depletion_obs_fract_root_depth: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 126
          Width = 490
          Height = 22
          Hint = 
            'Usually use a value 1 or less. Greater than 1 will evaluate deep' +
            'er than the roots.'
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Fraction of current root depth'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
        object edit_depletion_observe_depth: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 5
          Top = 98
          Width = 490
          Height = 22
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Depletion observation depth'
          TabOrder = 2
        end
        object panel_observation_depth_model: TPanel
          Left = 2
          Top = 43
          Width = 496
          Height = 52
          Align = alTop
          BevelOuter = bvNone
          TabOrder = 3
          object depletion_contigency_note: TLabel
            Left = 401
            Top = 0
            Width = 95
            Height = 52
            Align = alClient
            AutoSize = False
            Caption = 
              'These options can be set because this event has postponable cont' +
              'igency.'
            Color = clInfoBk
            ParentColor = False
            WordWrap = True
          end
          object radiobutton_depletion_observation_depth_mode: TRNAutoRadioGroup
            Left = 0
            Top = 0
            Width = 401
            Height = 52
            Align = alLeft
            Caption = 'Observation depth mode'
            Columns = 2
            Items.Strings = (
              'at a constant fixed depth'
              'at a fraction of the current root depth'
              'the entire profile')
            TabOrder = 0
            OnClick = radiobutton_depletion_observation_depth_modeClick
          end
        end
      end
      object edit_leaf_water_potential: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 3
        Top = 53
        Width = 494
        Height = 21
        Hint = 
          'Irrigation will occur when the leaf water potential becomes more' +
          ' negative than this value.'
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Leaf water potential'
        TabOrder = 2
      end
    end
    object TabSheet2: TTabSheet
      Caption = 'Application amount'
      ImageIndex = 1
      ExplicitHeight = 263
      object panel_application_mode: TPanel
        Left = 0
        Top = 0
        Width = 500
        Height = 51
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 0
        object note_depletion_constraints: TLabel
          Left = 185
          Top = 0
          Width = 315
          Height = 51
          Align = alClient
          AutoSize = False
          Caption = 
            'If this operation is schedule to repeat, the "Irrigate to a spec' +
            'ified refill point" (based on soil conditions) may be useful.  I' +
            'f this is a single event, simply use "Irrigate a fixed amount".'
          Color = clInfoBk
          ParentColor = False
          WordWrap = True
        end
        object radiogroup_application_mode: TRNAutoRadioGroup
          Left = 0
          Top = 0
          Width = 185
          Height = 51
          Align = alLeft
          Caption = 'Application mode'
          Items.Strings = (
            'Irrigate a fixed amount'
            'Irrigate to a specified refill point')
          TabOrder = 0
          OnClick = application_mode_radiogroup_onclick
        end
      end
      object edit_amount: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 3
        Top = 54
        Width = 494
        Height = 22
        Hint = 
          'Enter 0.0 to supply enough water to refill to plant available wa' +
          'ter'
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Total irrigation'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 1
      end
      object panel_refill: TPanel
        Left = 0
        Top = 107
        Width = 500
        Height = 130
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 2
        ExplicitTop = 79
        object edit_net_irrigation_mult: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 3
          Top = 31
          Width = 494
          Height = 22
          Hint = 
            'This parameter is used to add a little more irrigation to compen' +
            'sate for inefficiency.  (Values > 1.0 are less efficient)'
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Net irrigation multiplier (1.0 is 100% efficient)'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 0
        end
        object edit_refill_point: TRNAutoParameterEditBar
          AlignWithMargins = True
          Left = 3
          Top = 3
          Width = 494
          Height = 22
          Hint = 
            'This is the fraction of plant available water to refill to (0=pe' +
            'rmanent wilt point, 1=field capacity)'
          Align = alTop
          Alignment = taLeftJustify
          BevelOuter = bvNone
          Caption = 'Refill point'
          ParentShowHint = False
          ShowHint = True
          TabOrder = 1
        end
        object groupbox_system_capacity: TGroupBox
          Left = 0
          Top = 56
          Width = 500
          Height = 74
          Align = alTop
          Caption = 'System capacity'
          TabOrder = 2
          object edit_max_application: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 18
            Width = 490
            Height = 22
            Hint = 
              'This can be used to fix an upper limit a recharging option is sp' +
              'ecified (Enter 0.0 if no maximum is applied)'
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Maximum application'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 0
          end
          object edit_min_application: TRNAutoParameterEditBar
            AlignWithMargins = True
            Left = 5
            Top = 46
            Width = 490
            Height = 22
            Hint = 
              'This can be used to specify the minimum amount of water the irri' +
              'gation system can deliver (Enter 0.0 if no maximum is applied)'
            Align = alTop
            Alignment = taLeftJustify
            BevelOuter = bvNone
            Caption = 'Minimum application'
            ParentShowHint = False
            ShowHint = True
            TabOrder = 1
          end
        end
      end
      object edit_wetting_fraction: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 3
        Top = 82
        Width = 494
        Height = 22
        Hint = 'Used for drip or other directed irrigation systems'
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Wetting surface area fraction'
        ParentShowHint = False
        ShowHint = True
        TabOrder = 3
        ExplicitLeft = -13
        ExplicitTop = 68
      end
    end
    object TabSheet3: TTabSheet
      Caption = 'Solute concentrations'
      ImageIndex = 2
      ExplicitHeight = 263
      object label_not_appl_0: TLabel
        Left = 0
        Top = 249
        Width = 500
        Height = 13
        Align = alBottom
        AutoSize = False
        Caption = 'Enter 0 values for concentration if not applicable.'
        Color = clInfoBk
        ParentColor = False
        WordWrap = True
        ExplicitTop = 225
      end
      object edit_salinity: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 3
        Top = 3
        Width = 494
        Height = 22
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Salt concentration (Salinity)'
        TabOrder = 0
      end
      object edit_NO3_N_conc: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 3
        Top = 31
        Width = 494
        Height = 22
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Nitrate NO3 nitrogen concentration'
        TabOrder = 1
      end
      object edit_NH4_N_conc: TRNAutoParameterEditBar
        AlignWithMargins = True
        Left = 3
        Top = 59
        Width = 494
        Height = 22
        Align = alTop
        Alignment = taLeftJustify
        BevelOuter = bvNone
        Caption = 'Ammonium NH4 nitrogen concentration'
        TabOrder = 2
      end
    end
  end
end
