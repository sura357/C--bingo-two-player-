object Form6: TForm6
  Left = 0
  Top = 0
  BorderStyle = bsSingle
  Caption = 'Form6'
  ClientHeight = 426
  ClientWidth = 568
  Color = clBtnFace
  Font.Charset = MAC_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 14
  object Label2: TLabel
    Left = 8
    Top = 8
    Width = 5
    Height = 22
    Font.Charset = MAC_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label1: TLabel
    Left = 8
    Top = 315
    Width = 9
    Height = 39
    Color = clGradientActiveCaption
    Font.Charset = MAC_CHARSET
    Font.Color = clWindowText
    Font.Height = -32
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentColor = False
    ParentFont = False
  end
  object GroupBox1: TGroupBox
    Left = 1060
    Top = 189
    Width = 249
    Height = 177
    Caption = 'Receive'
    TabOrder = 0
    object Memo1: TMemo
      Left = 3
      Top = 16
      Width = 243
      Height = 154
      Enabled = False
      Lines.Strings = (
        'Memo1')
      ScrollBars = ssBoth
      TabOrder = 0
    end
  end
  object GroupBox2: TGroupBox
    Left = 1060
    Top = 8
    Width = 249
    Height = 178
    Caption = 'Send'
    TabOrder = 1
    object Memo2: TMemo
      Left = 3
      Top = 20
      Width = 243
      Height = 155
      Enabled = False
      Lines.Strings = (
        'Memo2')
      ScrollBars = ssBoth
      TabOrder = 0
    end
  end
  object ComboBox1: TComboBox
    Left = 1092
    Top = 8
    Width = 145
    Height = 22
    TabOrder = 2
    Text = 'ComboBox1'
  end
  object Memo3: TMemo
    Left = 184
    Top = 321
    Width = 376
    Height = 97
    ScrollBars = ssBoth
    TabOrder = 3
  end
  object Button1: TButton
    Left = 485
    Top = 8
    Width = 75
    Height = 25
    Caption = #37325#26032#38283#22987
    TabOrder = 4
    Visible = False
    OnClick = Button1Click
  end
  object ClientSocket1: TClientSocket
    Active = False
    ClientType = ctNonBlocking
    Host = '1024'
    Port = 1024
    OnConnect = ClientSocket1Connect
    OnDisconnect = ClientSocket1Disconnect
    OnRead = ClientSocket1Read
    OnError = ClientSocket1Error
    Left = 168
  end
  object ServerSocket1: TServerSocket
    Active = False
    Port = 1024
    ServerType = stNonBlocking
    OnAccept = ServerSocket1Accept
    OnClientConnect = ServerSocket1ClientConnect
    OnClientDisconnect = ServerSocket1ClientDisconnect
    OnClientRead = ServerSocket1ClientRead
    OnClientError = ServerSocket1ClientError
    Left = 88
  end
  object MainMenu1: TMainMenu
    Left = 16
    object file1: TMenuItem
      Caption = 'play'
      object listen: TMenuItem
        Caption = 'Listen'
        OnClick = listenClick
      end
      object Connect1: TMenuItem
        Caption = 'Connect'
        OnClick = Connect1Click
      end
      object Disconnect1: TMenuItem
        Caption = 'DisConnect'
        OnClick = Disconnect1Click
      end
    end
  end
  object IdIPWatch1: TIdIPWatch
    Active = False
    HistoryFilename = 'iphist.dat'
    Left = 296
  end
  object Timer1: TTimer
    Enabled = False
    OnTimer = Timer1Timer
    Left = 232
  end
end
