namespace WindowsFormsApplication1
{
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.IO;
    using System.Windows.Forms;

    public class Form1 : Form
    {
        private bool alreadyloaded;
        private Button button1;
        private Button button2;
        private Button button3;
        private Button button4;
        private Button button5;
        private Button button6;
        private Button button7;
        private const int CBA_ADD = 0x20;
        private const int CBA_AND = 11;
        private const int CBA_GT = 0x23;
        private const int CBA_IF_FALSE = 10;
        private const int CBA_IF_KEYS_PRESSED = 7;
        private const int CBA_IF_TRUE = 8;
        private const int CBA_LT = 0x22;
        private const int CBA_OR = 0x21;
        private const int CBA_SLIDE_CODE = 9;
        private const int CBA_SUPER = 0x24;
        private TextBox cheatdatei;
        private byte[] cheatlist = new byte[0xaf0];
        private OpenFileDialog cheatopen;
        private const int CHEATS_16_BIT_WRITE = 0x72;
        private const int CHEATS_32_BIT_WRITE = 0x73;
        private byte[] cheatsCBACurrentSeed = new byte[12];
        private int cheatsCBAGetCountcount;
        private uint[] cheatsCBASeed = new uint[4];
        private byte[] cheatsCBASeedBuffer = new byte[0x30];
        private bool cheatsCBAShouldDecryptworkaround;
        private ushort[] cheatsCBATable = new ushort[0x100];
        private bool cheatsCBATableGenerated;
        private uint cheatsCBATemporaryValue;
        private int cheatsNumber;
        private byte[] codebuff = new byte[0x4000];
        private IContainer components;
        private CheckBox cv3;
        public bool douserpatchs;
        private TextBox gbapf;
        private const int GSA_16_BIT_ADD = 0x29;
        private const int GSA_16_BIT_FILL = 0x18;
        private const int GSA_16_BIT_GS_WRITE = 5;
        private const int GSA_16_BIT_GS_WRITE2 = 13;
        private const int GSA_16_BIT_IF_AND = 50;
        private const int GSA_16_BIT_IF_AND2 = 0x3b;
        private const int GSA_16_BIT_IF_AND3 = 0x61;
        private const int GSA_16_BIT_IF_FALSE2 = 0x1d;
        private const int GSA_16_BIT_IF_FALSE3 = 0x52;
        private const int GSA_16_BIT_IF_HIGHER_OR_EQ_U = 0x6b;
        private const int GSA_16_BIT_IF_HIGHER_S = 0x43;
        private const int GSA_16_BIT_IF_HIGHER_S2 = 0x49;
        private const int GSA_16_BIT_IF_HIGHER_S3 = 0x58;
        private const int GSA_16_BIT_IF_HIGHER_U = 0x2f;
        private const int GSA_16_BIT_IF_HIGHER_U2 = 0x38;
        private const int GSA_16_BIT_IF_HIGHER_U3 = 0x5e;
        private const int GSA_16_BIT_IF_LOWER_OR_EQ_U = 0x6a;
        private const int GSA_16_BIT_IF_LOWER_S = 0x40;
        private const int GSA_16_BIT_IF_LOWER_S2 = 70;
        private const int GSA_16_BIT_IF_LOWER_S3 = 0x55;
        private const int GSA_16_BIT_IF_LOWER_U = 0x2c;
        private const int GSA_16_BIT_IF_LOWER_U2 = 0x35;
        private const int GSA_16_BIT_IF_LOWER_U3 = 0x5b;
        private const int GSA_16_BIT_IF_TRUE2 = 0x1a;
        private const int GSA_16_BIT_IF_TRUE3 = 0x4f;
        private const int GSA_16_BIT_MIF_FALSE = 0x6d;
        private const int GSA_16_BIT_MIF_HIGHER_OR_EQ_U = 0x6f;
        private const int GSA_16_BIT_MIF_LOWER_OR_EQ_U = 110;
        private const int GSA_16_BIT_MIF_TRUE = 0x6c;
        private const int GSA_16_BIT_POINTER = 0x26;
        private const int GSA_16_BIT_ROM_PATCH = 3;
        private const int GSA_16_BIT_ROM_PATCH2C = 15;
        private const int GSA_16_BIT_ROM_PATCH2D = 100;
        private const int GSA_16_BIT_ROM_PATCH2E = 0x65;
        private const int GSA_16_BIT_ROM_PATCH2F = 0x66;
        private const int GSA_16_BIT_SLIDE = 0x11;
        private const int GSA_16_BIT_WRITE_IOREGS = 0x4b;
        private const int GSA_32_BIT_ADD = 0x2a;
        private const int GSA_32_BIT_ADD2 = 0x68;
        private const int GSA_32_BIT_GS_WRITE = 6;
        private const int GSA_32_BIT_GS_WRITE2 = 14;
        private const int GSA_32_BIT_IF_AND = 0x33;
        private const int GSA_32_BIT_IF_AND2 = 60;
        private const int GSA_32_BIT_IF_AND3 = 0x62;
        private const int GSA_32_BIT_IF_FALSE = 0x16;
        private const int GSA_32_BIT_IF_FALSE2 = 30;
        private const int GSA_32_BIT_IF_FALSE3 = 0x53;
        private const int GSA_32_BIT_IF_HIGHER_S = 0x44;
        private const int GSA_32_BIT_IF_HIGHER_S2 = 0x4a;
        private const int GSA_32_BIT_IF_HIGHER_S3 = 0x59;
        private const int GSA_32_BIT_IF_HIGHER_U = 0x30;
        private const int GSA_32_BIT_IF_HIGHER_U2 = 0x39;
        private const int GSA_32_BIT_IF_HIGHER_U3 = 0x5f;
        private const int GSA_32_BIT_IF_LOWER_S = 0x41;
        private const int GSA_32_BIT_IF_LOWER_S2 = 0x47;
        private const int GSA_32_BIT_IF_LOWER_S3 = 0x56;
        private const int GSA_32_BIT_IF_LOWER_U = 0x2d;
        private const int GSA_32_BIT_IF_LOWER_U2 = 0x36;
        private const int GSA_32_BIT_IF_LOWER_U3 = 0x5c;
        private const int GSA_32_BIT_IF_TRUE = 20;
        private const int GSA_32_BIT_IF_TRUE2 = 0x1b;
        private const int GSA_32_BIT_IF_TRUE3 = 80;
        private const int GSA_32_BIT_POINTER = 0x27;
        private const int GSA_32_BIT_SLIDE = 0x12;
        private const int GSA_32_BIT_SUB2 = 0x69;
        private const int GSA_32_BIT_WRITE_IOREGS = 0x4c;
        private const int GSA_8_BIT_ADD = 40;
        private const int GSA_8_BIT_FILL = 0x17;
        private const int GSA_8_BIT_GS_WRITE = 4;
        private const int GSA_8_BIT_GS_WRITE2 = 12;
        private const int GSA_8_BIT_IF_AND = 0x31;
        private const int GSA_8_BIT_IF_AND2 = 0x3a;
        private const int GSA_8_BIT_IF_AND3 = 0x60;
        private const int GSA_8_BIT_IF_FALSE = 0x15;
        private const int GSA_8_BIT_IF_FALSE2 = 0x1c;
        private const int GSA_8_BIT_IF_FALSE3 = 0x51;
        private const int GSA_8_BIT_IF_HIGHER_S = 0x42;
        private const int GSA_8_BIT_IF_HIGHER_S2 = 0x48;
        private const int GSA_8_BIT_IF_HIGHER_S3 = 0x57;
        private const int GSA_8_BIT_IF_HIGHER_U = 0x2e;
        private const int GSA_8_BIT_IF_HIGHER_U2 = 0x37;
        private const int GSA_8_BIT_IF_HIGHER_U3 = 0x5d;
        private const int GSA_8_BIT_IF_LOWER_S = 0x3f;
        private const int GSA_8_BIT_IF_LOWER_S2 = 0x45;
        private const int GSA_8_BIT_IF_LOWER_S3 = 0x54;
        private const int GSA_8_BIT_IF_LOWER_U = 0x2b;
        private const int GSA_8_BIT_IF_LOWER_U2 = 0x34;
        private const int GSA_8_BIT_IF_LOWER_U3 = 90;
        private const int GSA_8_BIT_IF_TRUE = 0x13;
        private const int GSA_8_BIT_IF_TRUE2 = 0x19;
        private const int GSA_8_BIT_IF_TRUE3 = 0x4e;
        private const int GSA_8_BIT_POINTER = 0x25;
        private const int GSA_8_BIT_SLIDE = 0x10;
        private const int GSA_ALWAYS = 0x3d;
        private const int GSA_ALWAYS2 = 0x3e;
        private const int GSA_ALWAYS3 = 0x63;
        private const int GSA_CODES_ON = 0x4d;
        private const int GSA_GROUP_WRITE = 0x67;
        private const int GSA_SLOWDOWN = 0x1f;
        private TextBox HBfiletext;
        private OpenFileDialog HBopen;
        private const int INT_16_BIT_WRITE = 1;
        private const int INT_32_BIT_WRITE = 2;
        private const int INT_8_BIT_WRITE = 0;
        private TextBox ipspf;
        private TextBox irqstack;
        private Label label1;
        private Label label10;
        private Label label11;
        private Label label2;
        private Label label3;
        private Label label4;
        private Label label5;
        private Label label6;
        private Label label7;
        private Label label8;
        private Label label9;
        private const int MASTER_CODE = 0x70;
        private OpenFileDialog openFileDialoggba;
        private OpenFileDialog openFileDialogpatch;
        private OpenFileDialog openFileDialogpatch2;
        private byte[] originalfile = new byte[0x2000000];
        private byte[] patchbuff = new byte[0x1000];
        private byte[] patchedfile = new byte[0x2000000];
        public byte[] patcheins = new byte[] { 
            0x12, 0, 160, 0xe3, 0, 240, 0x29, 0xe1, 40, 0xd0, 0x9f, 0xe5, 0x1f, 0, 160, 0xe3, 
            0, 240, 0x29, 0xe1, 0x18, 0xd0, 0x9f, 0xe5, 0x1c, 0x10, 0x9f, 0xe5, 0x20, 0, 0x8f, 0xe2, 
            0, 0, 0x81, 0xe5, 20, 0x10, 0x9f, 0xe5, 15, 0xe0, 160, 0xe1, 0x11, 0xff, 0x2f, 0xe1, 
            0xf2, 0xff, 0xff, 0xea
         };
        public byte[] patcheinsandplat = new byte[] { 
            0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xff, 
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 
            0, 0, 0xff, 0xff, 0, 0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
            0xff, 0xff, 0xff, 0xff
         };
        private TextBox patchersp;
        private TextBox readedirqspT;
        private TextBox readeduserspT;
        private SaveFileDialog saveFileDialog1;
        public uint sppatcher;
        private CheckBox spwechseln;
        private int super;
        private MaskedTextBox teilgr;
        public uint tempnum;
        private const int UNKNOWN_CODE = -1;
        public uint userpatchsnum;
        public uint userpatchsnumstat;
        private TextBox userstack;

        public Form1()
        {
            this.InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (this.openFileDialoggba.ShowDialog() == DialogResult.OK)
            {
                this.gbapf.Text = this.openFileDialoggba.FileName;
                FileStream stream = new FileStream(this.openFileDialoggba.FileName, FileMode.Open);
                byte[] buffer = new byte[0x2000000];
                stream.Read(buffer, 0, buffer.Length);
                uint user = 0;
                uint irq = 0;
                this.Patchspsearch(ref buffer, ref irq, ref user);
                this.readedirqspT.Text = Convert.ToString((long) irq, 0x10);
                this.readeduserspT.Text = Convert.ToString((long) user, 0x10);
                stream.Close();
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            uint tostr = 0;
            this.tempnum = (uint) (0x200 * (Convert.ToByte(this.teilgr.Text) - Convert.ToByte("0")));
            if (this.spwechseln.Checked || (this.ipspf.Text != ""))
            {
                FileStream stream;
                try
                {
                    stream = new FileStream(this.gbapf.Text, FileMode.Open);
                }
                catch (InvalidCastException exception)
                {
                    MessageBox.Show(exception.Message, "error", MessageBoxButtons.OK, MessageBoxIcon.Hand);
                    return;
                }
                stream.Read(this.originalfile, 0, 0x2000000);
                if (!this.alreadyloaded)
                {
                    this.originalfile.CopyTo(this.patchedfile, 0);
                }
                stream.Close();
                if (this.ipspf.Text != "")
                {
                    FileStream stream2;
                    try
                    {
                        stream2 = new FileStream(this.ipspf.Text, FileMode.Open);
                    }
                    catch (InvalidCastException exception2)
                    {
                        MessageBox.Show(exception2.Message, "error", MessageBoxButtons.OK, MessageBoxIcon.Hand);
                        return;
                    }
                    if (this.checkandpossips(stream2))
                    {
                        this.patchips(stream2, ref this.patchedfile);
                    }
                    else
                    {
                        MessageBox.Show("error not a ipsfile", "error", MessageBoxButtons.OK, MessageBoxIcon.Hand);
                    }
                    stream2.Close();
                }
                if (this.spwechseln.Checked)
                {
                    this.Patchsp(ref this.patchedfile);
                }
                tostr = this.findnumofpatches(this.originalfile, this.patchedfile);
            }
            if (this.cheatsNumber != 0)
            {
                tostr++;
            }
            if (this.saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                Stream str = this.saveFileDialog1.OpenFile();
                str.WriteByte(Convert.ToByte('f'));
                str.WriteByte(Convert.ToByte('l'));
                str.WriteByte(Convert.ToByte('y'));
                str.WriteByte(Convert.ToByte('g'));
                str.WriteByte(Convert.ToByte('b'));
                str.WriteByte(Convert.ToByte('a'));
                str.WriteByte(Convert.ToByte('p'));
                str.WriteByte(Convert.ToByte('a'));
                str.WriteByte(Convert.ToByte('t'));
                str.WriteByte(Convert.ToByte('c'));
                str.WriteByte(Convert.ToByte('h'));
                str.WriteByte(0);
                str.WriteByte((byte) (Convert.ToByte(this.teilgr.Text) - Convert.ToByte("0")));
                str.WriteByte(0);
                str.WriteByte(0);
                str.WriteByte(0);
                if (this.HBfiletext.Text != "")
                {
                    tostr++;
                }
                if (this.douserpatchs)
                {
                    this.storu32((tostr + this.userpatchsnum) + 1, ref str);
                }
                else
                {
                    this.storu32(tostr, ref str);
                }
                for (int i = 20; i < 80; i++)
                {
                    str.WriteByte(0);
                }
                if (this.HBfiletext.Text != "")
                {
                    tostr--;
                }
                int position = (int) str.Position;
                int num4 = (int) (tostr * 12);
                if (this.douserpatchs)
                {
                    num4 += ((int) (13 * this.userpatchsnum)) + 12;
                }
                for (int j = 0; j < num4; j++)
                {
                    str.WriteByte(0);
                }
                int num6 = (0x200 * (Convert.ToByte(this.teilgr.Text) - Convert.ToByte("0"))) - 1;
                while ((str.Position & num6) != 0L)
                {
                    str.WriteByte(0);
                }
                int patchnumm = 0;
                while (this.writeichflypatch(ref position, ref patchnumm, ref str, this.originalfile, this.patchedfile))
                {
                }
                if (this.HBfiletext.Text != "")
                {
                    this.writeHBpatch(ref position, ref patchnumm, ref str);
                }
                if (this.cheatsNumber != 0)
                {
                    this.writeichflycheat(ref position, ref patchnumm, ref str, this.cheatsNumber, this.cheatlist);
                }
                if (this.douserpatchs)
                {
                    this.writeichflyuserpatches(ref position, ref patchnumm, ref str);
                }
                MessageBox.Show("Erfolgreich " + patchnumm.ToString() + " patches erstellt");
                str.Close();
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if (this.openFileDialogpatch.ShowDialog() == DialogResult.OK)
            {
                this.ipspf.Text = this.openFileDialogpatch.FileName;
            }
        }

        private void button4_Click(object sender, EventArgs e)
        {
            this.cheatsVerifyCheatCode(this.cheatdatei.Text);
        }

        private void button5_Click(object sender, EventArgs e)
        {
            if (this.cheatopen.ShowDialog() == DialogResult.OK)
            {
                this.cheatdatei.Text = this.cheatopen.FileName;
            }
        }

        private void button6_Click(object sender, EventArgs e)
        {
            FileStream stream;
            this.sppatcher = (uint) Convert.ToInt32(this.patchersp.Text, 0x10);
            Form2 form = new Form2();
            form.Show();
            try
            {
                stream = new FileStream(this.gbapf.Text, FileMode.Open);
            }
            catch (InvalidCastException exception)
            {
                MessageBox.Show(exception.Message, "error", MessageBoxButtons.OK, MessageBoxIcon.Hand);
                return;
            }
            stream.Read(this.originalfile, 0, 0x2000000);
            if (!this.alreadyloaded)
            {
                this.originalfile.CopyTo(this.patchedfile, 0);
            }
            this.alreadyloaded = true;
            stream.Close();
            form.laodin(this.gbapf.Text, this.ipspf.Text, ref this.originalfile, ref this.patchedfile, ref this.codebuff, ref this.patchbuff, ref this.userpatchsnum, this);
        }

        private void button7_Click(object sender, EventArgs e)
        {
            if (this.HBopen.ShowDialog() == DialogResult.OK)
            {
                this.HBfiletext.Text = this.HBopen.FileName;
            }
        }

        public bool CHEAT_IS_HEX(char a)
        {
            return (((a >= 'A') && (a <= 'F')) || ((a >= '0') && (a <= '9')));
        }

        private void cheatsAdd(string codeStr, uint rawaddress, uint address, uint value, int code, int size)
        {
            if (this.cheatsNumber < 100)
            {
                int cheatsNumber = this.cheatsNumber;
                this.storu32((uint) size, ref this.cheatlist, (uint) (cheatsNumber * 0x1c));
                this.storu32(rawaddress, ref this.cheatlist, (uint) ((cheatsNumber * 0x1c) + 12));
                this.storu32(address, ref this.cheatlist, (uint) ((cheatsNumber * 0x1c) + 0x10));
                this.storu32(value, ref this.cheatlist, (uint) ((cheatsNumber * 0x1c) + 20));
                this.storu32(uint.MaxValue, ref this.cheatlist, (uint) ((cheatsNumber * 0x1c) + 8));
                this.storu32(0, ref this.cheatlist, (uint) ((cheatsNumber * 0x1c) + 4));
                this.cheatsNumber++;
            }
        }

        private void cheatsAddCBACode(string code)
        {
            if (code.Length != 13)
            {
                MessageBox.Show("Invalid CBA code. Format is XXXXXXXX YYYY.");
            }
            else
            {
                int num;
                for (num = 0; num < 8; num++)
                {
                    if (!this.CHEAT_IS_HEX(code[num]))
                    {
                        MessageBox.Show("Invalid CBA code. Format is XXXXXXXX YYYY.");
                        return;
                    }
                }
                if (code[8] != ' ')
                {
                    MessageBox.Show("Invalid CBA code. Format is XXXXXXXX YYYY.");
                }
                else
                {
                    for (num = 9; num < 13; num++)
                    {
                        if (!this.CHEAT_IS_HEX(code[num]))
                        {
                            MessageBox.Show("Invalid CBA code. Format is XXXXXXXX YYYY.");
                            return;
                        }
                    }
                    char[] destinationArray = new char[8];
                    Array.Copy(code.ToCharArray(), destinationArray, 8);
                    string str = new string(destinationArray);
                    uint rawaddress = Convert.ToUInt32(str, 0x10);
                    char[] chArray2 = new char[4];
                    Array.Copy(code.ToCharArray(), 9, chArray2, 0, 4);
                    string str2 = new string(chArray2);
                    uint num3 = Convert.ToUInt32(str2, 0x10);
                    byte[] buffer3 = new byte[8];
                    buffer3[0] = (byte) (rawaddress & 0xff);
                    buffer3[1] = (byte) ((rawaddress >> 8) & 0xff);
                    buffer3[2] = (byte) ((rawaddress >> 0x10) & 0xff);
                    buffer3[3] = (byte) ((rawaddress >> 0x18) & 0xff);
                    buffer3[4] = (byte) (num3 & 0xff);
                    buffer3[5] = (byte) ((num3 >> 8) & 0xff);
                    byte[] buffer = buffer3;
                    rawaddress = buffer[0];
                    rawaddress += (uint) (buffer[1] << 8);
                    rawaddress += (uint) (buffer[2] << 0x10);
                    rawaddress += (uint) (buffer[3] << 0x18);
                    num3 = buffer[4];
                    num3 += (uint) (buffer[5] << 8);
                    uint num4 = (rawaddress >> 0x1c) & 15;
                    if (this.isMultilineWithData(this.cheatsNumber - 1) || (this.super > 0))
                    {
                        this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x200, -1);
                        if (this.super > 0)
                        {
                            this.super--;
                        }
                    }
                    else
                    {
                        switch (num4)
                        {
                            case 0:
                                if (!this.cheatsCBATableGenerated)
                                {
                                    this.cheatsCBAGenTable();
                                }
                                if (this.gbapf.Text != "")
                                {
                                    byte[] buffer2 = new byte[0x10000];
                                    using (FileStream stream = new FileStream(this.gbapf.Text, FileMode.Open))
                                    {
                                        stream.Read(buffer2, 0, 0x10000);
                                    }
                                    if (this.cheatsCBACalcCRC(buffer2, 0x10000) != rawaddress)
                                    {
                                        MessageBox.Show("Warning: Codes seem to be for a different game.\nCodes may not work correctly.");
                                    }
                                }
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x200, -1);
                                return;

                            case 1:
                                this.cheatsAdd(code, rawaddress, (rawaddress & 0x1ffffff) | 0x8000000, num3, 0x200, 0x70);
                                return;

                            case 2:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 0x21);
                                return;

                            case 3:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x200, 0);
                                return;

                            case 4:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 9);
                                return;

                            case 5:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 0x24);
                                this.super = this.getCodeLength(this.cheatsNumber - 1);
                                return;

                            case 6:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 11);
                                return;

                            case 7:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 8);
                                return;

                            case 8:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 1);
                                return;

                            case 10:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 10);
                                return;

                            case 11:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 0x23);
                                return;

                            case 12:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 0x22);
                                return;

                            case 13:
                                if ((rawaddress & 240) < 0x30)
                                {
                                    this.cheatsAdd(code, rawaddress, rawaddress & 240, num3, 0x200, 7);
                                }
                                return;

                            case 14:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, ((num3 & 0x8000) == 0x8000) ? (num3 | 0xffff0000) : num3, 0x200, 0x20);
                                return;

                            case 15:
                                this.cheatsAdd(code, rawaddress, rawaddress & 0xffffffe, num3, 0x200, 50);
                                return;
                        }
                        this.cheatsAdd(code, rawaddress, rawaddress & uint.MaxValue, num3, 0x200, -1);
                    }
                }
            }
        }

        private void cheatsAddGSACode(string code, bool v3)
        {
            if (code.Length != 0x10)
            {
                MessageBox.Show("Invalid GSA code. Format is XXXXXXXXYYYYYYYY");
            }
            else
            {
                for (int i = 0; i < 0x10; i++)
                {
                    if (!this.CHEAT_IS_HEX(code[i]))
                    {
                        MessageBox.Show("Invalid GSA code. Format is XXXXXXXXYYYYYYYY");
                        return;
                    }
                }
                char[] destinationArray = new char[8];
                Array.Copy(code.ToCharArray(), destinationArray, 8);
                string str = new string(destinationArray);
                uint rawaddress = Convert.ToUInt32(str, 0x10);
                Array.Copy(code.ToCharArray(), 8, destinationArray, 0, 8);
                string str2 = new string(destinationArray);
                uint num3 = Convert.ToUInt32(str2, 0x10);
                if (num3 == 0x1dc0de)
                {
                    this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, v3 ? 0x101 : 0x100, -1);
                }
                else if (this.isMultilineWithData(this.cheatsNumber - 1))
                {
                    this.cheatsAdd(code, rawaddress, rawaddress, num3, v3 ? 0x101 : 0x100, -1);
                }
                else if (!v3)
                {
                    int size = ((int) (rawaddress >> 0x1c)) & 15;
                    switch (size)
                    {
                        case 0:
                        case 1:
                        case 2:
                            this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x100, size);
                            return;

                        case 3:
                        {
                            uint num11 = (rawaddress >> 0x10) & 0xff;
                            if (num11 > 0x20)
                            {
                                switch (num11)
                                {
                                    case 0x30:
                                        this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, rawaddress & 0xffff, 0x100, 0x2a);
                                        return;

                                    case 0x40:
                                        this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, ~(rawaddress & 0xffff) + 1, 0x100, 0x2a);
                                        return;

                                    case 80:
                                        this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x100, 0x68);
                                        return;

                                    case 0x60:
                                        this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x100, 0x69);
                                        return;
                                }
                            }
                            else
                            {
                                switch (num11)
                                {
                                    case 0:
                                        this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x100, 0x67);
                                        return;

                                    case 0x10:
                                        this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, rawaddress & 0xff, 0x100, 0x2a);
                                        return;

                                    case 0x20:
                                        this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, ~(rawaddress & 0xff) + 1, 0x100, 0x2a);
                                        return;
                                }
                            }
                            this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                            return;
                        }
                        case 6:
                            rawaddress = rawaddress << 1;
                            if (((num3 >> 0x18) & 0xff) != 0)
                            {
                                this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                                return;
                            }
                            this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3 & 0xffff, 0x100, 3);
                            return;

                        case 8:
                            switch (((rawaddress >> 20) & 15))
                            {
                                case 1:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xf0fffff, num3, 0x100, 4);
                                    return;

                                case 2:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xf0fffff, num3, 0x100, 5);
                                    return;

                                case 4:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xf0fffff, 0, 0x100, 6);
                                    return;

                                case 15:
                                    this.cheatsAdd(code, rawaddress, 0, num3 & 0xffff, 0x100, 0x1f);
                                    return;
                            }
                            this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                            return;

                        case 13:
                            if (rawaddress == 0xdeadface)
                            {
                                this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                                return;
                            }
                            switch (((num3 >> 20) & 15))
                            {
                                case 0:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3 & 0xffff, 0x100, 8);
                                    return;

                                case 1:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3 & 0xffff, 0x100, 10);
                                    return;

                                case 2:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3 & 0xffff, 0x100, 0x6a);
                                    return;

                                case 3:
                                    this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3 & 0xffff, 0x100, 0x6b);
                                    return;
                            }
                            this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                            return;

                        case 14:
                            switch (((num3 >> 0x1c) & 15))
                            {
                                case 0:
                                    this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, rawaddress & 0xffff, 0x100, 0x6c);
                                    return;

                                case 1:
                                    this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, rawaddress & 0xffff, 0x100, 0x6d);
                                    return;

                                case 2:
                                    this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, rawaddress & 0xffff, 0x100, 110);
                                    return;

                                case 3:
                                    this.cheatsAdd(code, rawaddress, num3 & 0xfffffff, rawaddress & 0xffff, 0x100, 0x6f);
                                    return;
                            }
                            this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                            return;

                        case 15:
                            this.cheatsAdd(code, rawaddress, rawaddress & 0xfffffff, num3, 0x100, 0x70);
                            return;
                    }
                    this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x100, -1);
                }
                else
                {
                    int num4 = (int) (((rawaddress >> 0x19) & 0x7f) | ((rawaddress >> 0x11) & 0x80));
                    uint address = ((uint) ((rawaddress & 0xf00000) << 4)) | (rawaddress & 0x3ffff);
                    ushort num6 = (ushort) ((rawaddress >> 0x18) & 0xff);
                    if ((num6 & 0xfe) == 0xc4)
                    {
                        this.cheatsAdd(code, rawaddress, (rawaddress & 0x1ffffff) | 0x8000000, num3, 0x101, 0x70);
                    }
                    else
                    {
                        switch (num4)
                        {
                            case 0:
                                if (rawaddress != 0)
                                {
                                    this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x17);
                                    return;
                                }
                                num4 = ((int) (num3 >> 0x19)) & 0x7f;
                                address = ((uint) ((num3 & 0xf00000) << 4)) | (num3 & 0x3ffff);
                                switch (num4)
                                {
                                    case 4:
                                        this.cheatsAdd(code, rawaddress, 0, num3 & 0xffffff, 0x101, 0x1f);
                                        return;

                                    case 8:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 12);
                                        return;

                                    case 9:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 13);
                                        return;

                                    case 10:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 14);
                                        return;

                                    case 12:
                                        this.cheatsAdd(code, rawaddress, 0, num3 & 0xffffff, 0x101, 15);
                                        return;

                                    case 13:
                                        this.cheatsAdd(code, rawaddress, 0, num3 & 0xffffff, 0x101, 100);
                                        return;

                                    case 14:
                                        this.cheatsAdd(code, rawaddress, 0, num3 & 0xffffff, 0x101, 0x65);
                                        return;

                                    case 15:
                                        this.cheatsAdd(code, rawaddress, 0, num3 & 0xffffff, 0x101, 0x66);
                                        return;

                                    case 0x20:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 0x4d);
                                        return;

                                    case 0x40:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 0x10);
                                        return;

                                    case 0x41:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 0x11);
                                        return;

                                    case 0x42:
                                        this.cheatsAdd(code, rawaddress, 0, address, 0x101, 0x12);
                                        return;
                                }
                                break;

                            case 1:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x18);
                                return;

                            case 2:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 2);
                                return;

                            case 4:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x13);
                                return;

                            case 5:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 8);
                                return;

                            case 6:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 20);
                                return;

                            case 7:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x3d);
                                return;

                            case 8:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x15);
                                return;

                            case 9:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 10);
                                return;

                            case 10:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x16);
                                return;

                            case 12:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x3f);
                                return;

                            case 13:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x40);
                                return;

                            case 14:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x41);
                                return;

                            case 0x10:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x42);
                                return;

                            case 0x11:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x43);
                                return;

                            case 0x12:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x44);
                                return;

                            case 20:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x2b);
                                return;

                            case 0x15:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x2c);
                                return;

                            case 0x16:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x2d);
                                return;

                            case 0x18:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x2e);
                                return;

                            case 0x19:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x2f);
                                return;

                            case 0x1a:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x30);
                                return;

                            case 0x1c:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x31);
                                return;

                            case 0x1d:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 50);
                                return;

                            case 30:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x33);
                                return;

                            case 0x20:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x25);
                                return;

                            case 0x21:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x26);
                                return;

                            case 0x22:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x27);
                                return;

                            case 0x24:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x19);
                                return;

                            case 0x25:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x1a);
                                return;

                            case 0x26:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x1b);
                                return;

                            case 0x27:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x3e);
                                return;

                            case 40:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x1c);
                                return;

                            case 0x29:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x1d);
                                return;

                            case 0x2a:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 30);
                                return;

                            case 0x2c:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x45);
                                return;

                            case 0x2d:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 70);
                                return;

                            case 0x2e:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x47);
                                return;

                            case 0x30:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x48);
                                return;

                            case 0x31:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x49);
                                return;

                            case 50:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x4a);
                                return;

                            case 0x34:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x34);
                                return;

                            case 0x35:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x35);
                                return;

                            case 0x36:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x36);
                                return;

                            case 0x38:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x37);
                                return;

                            case 0x39:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x38);
                                return;

                            case 0x3a:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x39);
                                return;

                            case 60:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x3a);
                                return;

                            case 0x3d:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x3b);
                                return;

                            case 0x3e:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 60);
                                return;

                            case 0x40:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 40);
                                return;

                            case 0x41:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x29);
                                return;

                            case 0x42:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x2a);
                                return;

                            case 0x44:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x4e);
                                return;

                            case 0x45:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x4f);
                                return;

                            case 70:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 80);
                                return;

                            case 0x47:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x63);
                                return;

                            case 0x48:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x51);
                                return;

                            case 0x49:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x52);
                                return;

                            case 0x4a:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x53);
                                return;

                            case 0x4c:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x54);
                                return;

                            case 0x4d:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x55);
                                return;

                            case 0x4e:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x56);
                                return;

                            case 80:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x57);
                                return;

                            case 0x51:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x58);
                                return;

                            case 0x52:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x59);
                                return;

                            case 0x54:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 90);
                                return;

                            case 0x55:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x5b);
                                return;

                            case 0x56:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x5c);
                                return;

                            case 0x58:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x5d);
                                return;

                            case 0x59:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x5e);
                                return;

                            case 90:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x5f);
                                return;

                            case 0x5c:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x60);
                                return;

                            case 0x5d:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x61);
                                return;

                            case 0x5e:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x62);
                                return;

                            case 0x63:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x4b);
                                return;

                            case 0xe3:
                                this.cheatsAdd(code, rawaddress, address, num3, 0x101, 0x4c);
                                return;

                            default:
                                this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x101, -1);
                                return;
                        }
                        this.cheatsAdd(code, rawaddress, rawaddress, num3, 0x101, -1);
                    }
                }
            }
        }

        private uint cheatsCBACalcCRC(byte[] rom, int count)
        {
            uint maxValue = uint.MaxValue;
            int num2 = 0;
            if ((count & 3) == 0)
            {
                count = (count >> 2) - 1;
                if (count != -1)
                {
                    while (count != -1)
                    {
                        maxValue = (((maxValue << 8) ^ this.cheatsCBATable[((maxValue << 0x10) >> 0x18) ^ rom[num2++]]) << 0x10) >> 0x10;
                        maxValue = (((maxValue << 8) ^ this.cheatsCBATable[((maxValue << 0x10) >> 0x18) ^ rom[num2++]]) << 0x10) >> 0x10;
                        maxValue = (((maxValue << 8) ^ this.cheatsCBATable[((maxValue << 0x10) >> 0x18) ^ rom[num2++]]) << 0x10) >> 0x10;
                        maxValue = (((maxValue << 8) ^ this.cheatsCBATable[((maxValue << 0x10) >> 0x18) ^ rom[num2++]]) << 0x10) >> 0x10;
                        count--;
                    }
                }
            }
            return (maxValue & 0xffff);
        }

        private uint cheatsCBAEncWorker()
        {
            uint num = (this.cheatsCBATemporaryValue * 0x41c64e6d) + 0x3039;
            uint num2 = (num * 0x41c64e6d) + 0x3039;
            uint num3 = num >> 0x10;
            num = (uint) (((num2 >> 0x10) & 0x7fff) << 15);
            num3 = (num3 << 30) | num;
            num = (num2 * 0x41c64e6d) + 0x3039;
            this.cheatsCBATemporaryValue = num;
            return (num3 | ((num >> 0x10) & 0x7fff));
        }

        private void cheatsCBAGenTable()
        {
            for (int i = 0; i < 0x100; i++)
            {
                this.cheatsCBATable[i] = this.cheatsCBAGenValue((uint) i, 0x1021, 0);
            }
            this.cheatsCBATableGenerated = true;
        }

        private ushort cheatsCBAGenValue(uint x, uint y, uint z)
        {
            y = y << 0x10;
            z = z << 0x10;
            x = x << 0x18;
            uint num = y >> 0x10;
            z = z >> 0x10;
            x = x >> 0x10;
            for (int i = 0; i < 8; i++)
            {
                uint num3 = z ^ x;
                if (num3 >= 0)
                {
                    num3 = z << 0x11;
                }
                else
                {
                    num3 = z << 1;
                    num3 ^= num;
                    num3 = num3 << 0x10;
                }
                z = num3 >> 0x10;
                num3 = x << 0x11;
                x = num3 >> 0x10;
            }
            return (ushort) (z & 0xffff);
        }

        private int cheatsCBAGetCount()
        {
            this.cheatsCBAGetCountcount++;
            return (this.cheatsCBAGetCountcount - 1);
        }

        private void cheatsCBAParseSeedCode(uint address, uint value, ref uint[] array)
        {
            array[0] = 1;
            array[1] = value & 0xff;
            array[2] = (address >> 0x10) & 0xff;
            array[3] = (value >> 8) & 0xff;
            array[4] = (address >> 0x18) & 15;
            array[5] = address & 0xffff;
            array[6] = address;
            array[7] = value;
        }

        private bool cheatsCBAShouldDecrypt()
        {
            return this.cheatsCBAShouldDecryptworkaround;
        }

        private bool cheatsVerifyCheatCode(string code)
        {
            int length = code.Length;
            if (length == 0x10)
            {
                this.cheatsAddGSACode(code, this.cv3.Checked);
                return true;
            }
            if (((length != 11) && (length != 13)) && (length != 0x11))
            {
                MessageBox.Show("Invalid cheat code" + code.ToString());
                return false;
            }
            if (code.ToCharArray()[8] != ':')
            {
                if ((code.ToCharArray()[8] == ' ') && (length == 13))
                {
                    this.cheatsAddCBACode(code);
                    return true;
                }
                code.ToCharArray();
                MessageBox.Show("Invalid cheat code" + code.ToString());
                return false;
            }
            int num2 = 0;
            while (num2 < 8)
            {
                if (!this.CHEAT_IS_HEX(code[num2]))
                {
                    MessageBox.Show("Invalid cheat code" + code.ToString());
                    return false;
                }
                num2++;
            }
            for (num2 = 9; num2 < length; num2++)
            {
                if (!this.CHEAT_IS_HEX(code[num2]))
                {
                    MessageBox.Show("Invalid cheat code" + code.ToString());
                    return false;
                }
            }
            uint rawaddress = 0;
            uint num4 = 0;
            char[] destinationArray = new char[8];
            Array.Copy(code.ToCharArray(), destinationArray, 8);
            string str = new string(destinationArray);
            rawaddress = Convert.ToUInt32(str, 0x10);
            switch ((rawaddress >> 0x18))
            {
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                {
                    Array.Copy(code.ToCharArray(), 9, destinationArray, 0, code.Length - 9);
                    string str2 = new string(destinationArray);
                    num4 = Convert.ToUInt32(str2, 0x10);
                    int num5 = 0;
                    switch (length)
                    {
                        case 13:
                            num5 = 0x72;
                            break;

                        case 0x11:
                            num5 = 0x73;
                            break;
                    }
                    this.cheatsAdd(code, rawaddress, rawaddress, num4, num5, num5);
                    return true;
                }
            }
            MessageBox.Show("Invalid cheat code address:" + rawaddress);
            return false;
        }

        public bool checkandpossips(FileStream str)
        {
            byte[] buffer = new byte[] { (byte) str.ReadByte(), (byte) str.ReadByte(), (byte) str.ReadByte(), (byte) str.ReadByte(), (byte) str.ReadByte() };
            if ((((buffer[0] != 80) || (buffer[1] != 0x41)) || ((buffer[2] != 0x54) || (buffer[3] != 0x43))) || (buffer[4] != 0x48))
            {
                return false;
            }
            str.Position = str.Length - 3L;
            if (this.readBYTE3_TO_UINT(ref str) != 0x454f46)
            {
                MessageBox.Show("Fehlendes EOF. Gehacktes oder nur teilweise forhandes ips.Patche nach m\x00f6glichkeit weiter. Entfernen sie nach M\x00f6glichkeit halbe Records und erstellen sie ein EOF", "info", MessageBoxButtons.OK, MessageBoxIcon.Asterisk);
            }
            str.Position = 5L;
            return true;
        }

        public bool Compararchuck(uint offset, byte[] first, byte[] secound)
        {
            for (uint i = offset; i < (offset + this.tempnum); i++)
            {
                if (first[i] != secound[i])
                {
                    return false;
                }
            }
            return true;
        }

        public bool Comparrawchuck(uint offset, byte[] first, byte[] secound, int size)
        {
            for (uint i = offset; i < (offset + size); i++)
            {
                if (first[i] != secound[i - offset])
                {
                    return false;
                }
            }
            return true;
        }

        public bool Comparrawchuck(uint offset, byte[] first, byte[] secound, int size, byte[] andplat)
        {
            for (uint i = offset; i < (offset + size); i++)
            {
                if ((first[i] & andplat[i - offset]) != (secound[i - offset] & andplat[i - offset]))
                {
                    return false;
                }
            }
            return true;
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        public uint findnumofpatches(byte[] originalfile, byte[] patchedfile)
        {
            int num = 0;
            int num2 = 0;
            while (num != 0x2000000)
            {
                if (!this.Compararchuck((uint) num, originalfile, patchedfile))
                {
                    num2++;
                }
                num += (int) this.tempnum;
            }
            return (uint) num2;
        }

        public uint gbareadBYTE4_TO_UINT(ref FileStream str)
        {
            uint num = (uint) str.ReadByte();
            uint num2 = (uint) str.ReadByte();
            uint num3 = (uint) str.ReadByte();
            uint num4 = (uint) str.ReadByte();
            return (((((num << 0x18) & 0xff000000) | ((num4 << 0x10) & 0xff0000)) | ((num3 << 8) & 0xff00)) | (num2 & 0xff));
        }

        public uint gbareadBYTE4_TO_UINT(byte[] fild, uint offset)
        {
            uint num = fild[offset];
            uint num2 = fild[(int) ((IntPtr) (offset + 1))];
            uint num3 = fild[(int) ((IntPtr) (offset + 2))];
            uint num4 = fild[(int) ((IntPtr) (offset + 3))];
            return (((((num4 << 0x18) & 0xff000000) | ((num3 << 0x10) & 0xff0000)) | ((num2 << 8) & 0xff00)) | (num & 0xff));
        }

        private int getCodeLength(int num)
        {
            if ((num < this.cheatsNumber) && (num >= 0))
            {
                switch (this.gbareadBYTE4_TO_UINT(this.cheatlist, (uint) (num * 0x1c)))
                {
                    case uint.MaxValue:
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 8:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 0x10:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 20:
                    case 0x15:
                    case 0x16:
                    case 0x17:
                    case 0x18:
                    case 0x19:
                    case 0x1a:
                    case 0x1b:
                    case 0x1c:
                    case 0x1d:
                    case 30:
                    case 0x1f:
                    case 0x20:
                    case 0x21:
                    case 0x22:
                    case 0x23:
                    case 0x25:
                    case 0x26:
                    case 0x27:
                    case 40:
                    case 0x29:
                    case 0x2a:
                    case 0x2b:
                    case 0x2c:
                    case 0x2d:
                    case 0x2e:
                    case 0x2f:
                    case 0x30:
                    case 0x31:
                    case 50:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                    case 0x3a:
                    case 0x3b:
                    case 60:
                    case 0x3d:
                    case 0x3e:
                    case 0x3f:
                    case 0x40:
                    case 0x41:
                    case 0x42:
                    case 0x43:
                    case 0x44:
                    case 0x45:
                    case 70:
                    case 0x47:
                    case 0x48:
                    case 0x49:
                    case 0x4a:
                    case 0x4b:
                    case 0x4c:
                    case 0x4d:
                    case 0x4e:
                    case 0x4f:
                    case 80:
                    case 0x51:
                    case 0x52:
                    case 0x53:
                    case 0x54:
                    case 0x55:
                    case 0x56:
                    case 0x57:
                    case 0x58:
                    case 0x59:
                    case 90:
                    case 0x5b:
                    case 0x5c:
                    case 0x5d:
                    case 0x5e:
                    case 0x5f:
                    case 0x60:
                    case 0x61:
                    case 0x62:
                    case 100:
                    case 0x65:
                    case 0x66:
                    case 0x67:
                    case 0x68:
                    case 0x69:
                    case 0x6a:
                    case 0x6b:
                    case 0x6c:
                    case 0x6d:
                    case 110:
                    case 0x6f:
                    case 0x70:
                    case 0x72:
                    case 0x73:
                        return 1;

                    case 7:
                    case 9:
                        return 2;

                    case 0x24:
                        return (((int) (((this.gbareadBYTE4_TO_UINT(this.cheatlist, (uint) ((num * 0x1c) + 20)) - 1) & 0xffff) / 3)) + 1);
                }
            }
            return 1;
        }

        public uint Getipsrecs(FileStream str)
        {
            uint num = 0;
            long position = str.Position;
            try
            {
            Label_0009:
                if (this.readBYTE3_TO_UINT(ref str) == 0x454f46)
                {
                    num++;
                }
                else
                {
                    uint num4 = this.readBYTE2_TO_UINT(ref str);
                    num++;
                    if (num4 == 0)
                    {
                        str.Seek(3L, SeekOrigin.Current);
                    }
                    else
                    {
                        str.Seek((long) num4, SeekOrigin.Current);
                    }
                    num++;
                    goto Label_0009;
                }
            }
            catch
            {
                num++;
            }
            str.Position = position;
            return num;
        }

        private void InitializeComponent()
        {
            this.gbapf = new TextBox();
            this.button1 = new Button();
            this.button2 = new Button();
            this.label1 = new Label();
            this.ipspf = new TextBox();
            this.button3 = new Button();
            this.label2 = new Label();
            this.spwechseln = new CheckBox();
            this.openFileDialoggba = new OpenFileDialog();
            this.openFileDialogpatch = new OpenFileDialog();
            this.saveFileDialog1 = new SaveFileDialog();
            this.cheatdatei = new TextBox();
            this.button5 = new Button();
            this.label4 = new Label();
            this.openFileDialogpatch2 = new OpenFileDialog();
            this.teilgr = new MaskedTextBox();
            this.label5 = new Label();
            this.irqstack = new TextBox();
            this.userstack = new TextBox();
            this.button4 = new Button();
            this.cheatopen = new OpenFileDialog();
            this.cv3 = new CheckBox();
            this.label3 = new Label();
            this.label6 = new Label();
            this.readedirqspT = new TextBox();
            this.readeduserspT = new TextBox();
            this.label7 = new Label();
            this.label8 = new Label();
            this.label9 = new Label();
            this.button6 = new Button();
            this.label10 = new Label();
            this.HBfiletext = new TextBox();
            this.button7 = new Button();
            this.HBopen = new OpenFileDialog();
            this.label11 = new Label();
            this.patchersp = new TextBox();
            base.SuspendLayout();
            this.gbapf.Location = new Point(0x44, 12);
            this.gbapf.Name = "gbapf";
            this.gbapf.Size = new Size(0x161, 20);
            this.gbapf.TabIndex = 0;
            this.button1.Location = new Point(0x1a9, 10);
            this.button1.Name = "button1";
            this.button1.Size = new Size(0x5d, 0x17);
            this.button1.TabIndex = 1;
            this.button1.Text = "Durchsuchen";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new EventHandler(this.button1_Click);
            this.button2.Location = new Point(12, 0xb0);
            this.button2.Name = "button2";
            this.button2.Size = new Size(0x1fa, 0x17);
            this.button2.TabIndex = 2;
            this.button2.Text = "Erstellen";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new EventHandler(this.button2_Click);
            this.label1.AutoSize = true;
            this.label1.Location = new Point(5, 15);
            this.label1.Name = "label1";
            this.label1.Size = new Size(0x39, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "GBA Datei";
            this.ipspf.Location = new Point(0x44, 40);
            this.ipspf.Name = "ipspf";
            this.ipspf.Size = new Size(0x161, 20);
            this.ipspf.TabIndex = 0;
            this.button3.Location = new Point(0x1a9, 0x26);
            this.button3.Name = "button3";
            this.button3.Size = new Size(0x5d, 0x17);
            this.button3.TabIndex = 1;
            this.button3.Text = "Durchsuchen";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new EventHandler(this.button3_Click);
            this.label2.AutoSize = true;
            this.label2.Location = new Point(5, 0x2b);
            this.label2.Name = "label2";
            this.label2.Size = new Size(0x34, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "IPS Datei";
            this.spwechseln.AutoSize = true;
            this.spwechseln.Location = new Point(12, 0x7b);
            this.spwechseln.Name = "spwechseln";
            this.spwechseln.Size = new Size(0x55, 0x11);
            this.spwechseln.TabIndex = 4;
            this.spwechseln.Text = "sp wechseln";
            this.spwechseln.UseVisualStyleBackColor = true;
            this.openFileDialoggba.DefaultExt = "gba";
            this.openFileDialogpatch.FileName = "openFileDialog2";
            this.saveFileDialog1.FileName = "patch.pat";
            this.cheatdatei.Location = new Point(0x44, 0x43);
            this.cheatdatei.Name = "cheatdatei";
            this.cheatdatei.Size = new Size(0x10b, 20);
            this.cheatdatei.TabIndex = 0;
            this.button5.Location = new Point(0x1a9, 0x42);
            this.button5.Name = "button5";
            this.button5.Size = new Size(0x5d, 0x17);
            this.button5.TabIndex = 1;
            this.button5.Text = "Durchsuchen";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new EventHandler(this.button5_Click);
            this.label4.AutoSize = true;
            this.label4.Location = new Point(5, 70);
            this.label4.Name = "label4";
            this.label4.Size = new Size(0x3e, 13);
            this.label4.TabIndex = 3;
            this.label4.Text = "cheat Datei";
            this.openFileDialogpatch2.FileName = "openFileDialog2";
            this.teilgr.Location = new Point(0x1a2, 0x79);
            this.teilgr.Name = "teilgr";
            this.teilgr.Size = new Size(100, 20);
            this.teilgr.TabIndex = 5;
            this.teilgr.Text = "1";
            this.label5.AutoSize = true;
            this.label5.Location = new Point(0x164, 0x7d);
            this.label5.Name = "label5";
            this.label5.Size = new Size(0x33, 13);
            this.label5.TabIndex = 6;
            this.label5.Text = "Teilgr\x00f6\x00dfe";
            this.irqstack.Location = new Point(0x7f, 0x79);
            this.irqstack.Name = "irqstack";
            this.irqstack.Size = new Size(60, 20);
            this.irqstack.TabIndex = 7;
            this.irqstack.Text = "027C2160";
            this.userstack.Location = new Point(0xe4, 0x79);
            this.userstack.Name = "userstack";
            this.userstack.Size = new Size(60, 20);
            this.userstack.TabIndex = 7;
            this.button4.Location = new Point(0x155, 0x42);
            this.button4.Name = "button4";
            this.button4.Size = new Size(80, 0x17);
            this.button4.TabIndex = 8;
            this.button4.Text = "Verarbeiten";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new EventHandler(this.button4_Click);
            this.cheatopen.FileName = "openFileDialog1";
            this.cv3.AutoSize = true;
            this.cv3.Location = new Point(0x137, 0x7c);
            this.cv3.Name = "cv3";
            this.cv3.Size = new Size(0x27, 0x11);
            this.cv3.TabIndex = 9;
            this.cv3.Text = "V3";
            this.cv3.UseVisualStyleBackColor = true;
            this.label3.AutoSize = true;
            this.label3.Location = new Point(0x67, 0x7d);
            this.label3.Name = "label3";
            this.label3.Size = new Size(0x12, 13);
            this.label3.TabIndex = 11;
            this.label3.Text = "irq";
            this.label6.AutoSize = true;
            this.label6.Location = new Point(0xc1, 0x7d);
            this.label6.Name = "label6";
            this.label6.Size = new Size(0x1d, 13);
            this.label6.TabIndex = 12;
            this.label6.Text = "User";
            this.readedirqspT.Location = new Point(0x7f, 0x94);
            this.readedirqspT.Name = "readedirqspT";
            this.readedirqspT.Size = new Size(60, 20);
            this.readedirqspT.TabIndex = 7;
            this.readeduserspT.Location = new Point(0xe4, 0x94);
            this.readeduserspT.Name = "readeduserspT";
            this.readeduserspT.Size = new Size(60, 20);
            this.readeduserspT.TabIndex = 7;
            this.label7.AutoSize = true;
            this.label7.Location = new Point(0x67, 0x98);
            this.label7.Name = "label7";
            this.label7.Size = new Size(0x12, 13);
            this.label7.TabIndex = 11;
            this.label7.Text = "irq";
            this.label8.AutoSize = true;
            this.label8.Location = new Point(0xc1, 0x98);
            this.label8.Name = "label8";
            this.label8.Size = new Size(0x1d, 13);
            this.label8.TabIndex = 12;
            this.label8.Text = "User";
            this.label9.AutoSize = true;
            this.label9.Location = new Point(9, 0x9b);
            this.label9.Name = "label9";
            this.label9.Size = new Size(0x2a, 13);
            this.label9.TabIndex = 13;
            this.label9.Text = "GBAfile";
            this.button6.Location = new Point(0x1b6, 0x95);
            this.button6.Name = "button6";
            this.button6.Size = new Size(80, 0x15);
            this.button6.TabIndex = 14;
            this.button6.Text = "Start Anzeige";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new EventHandler(this.button6_Click);
            this.label10.AutoSize = true;
            this.label10.Location = new Point(5, 0x61);
            this.label10.Name = "label10";
            this.label10.Size = new Size(50, 13);
            this.label10.TabIndex = 3;
            this.label10.Text = "HB Datei";
            this.HBfiletext.Location = new Point(0x44, 0x5d);
            this.HBfiletext.Name = "HBfiletext";
            this.HBfiletext.Size = new Size(0x161, 20);
            this.HBfiletext.TabIndex = 0;
            this.HBfiletext.TextChanged += new EventHandler(this.textBox1_TextChanged);
            this.button7.Location = new Point(0x1a9, 0x5b);
            this.button7.Name = "button7";
            this.button7.Size = new Size(0x5d, 0x17);
            this.button7.TabIndex = 1;
            this.button7.Text = "Durchsuchen";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Click += new EventHandler(this.button7_Click);
            this.HBopen.FileName = "openFileDialog1";
            this.label11.AutoSize = true;
            this.label11.Location = new Point(0x126, 0x9b);
            this.label11.Name = "label11";
            this.label11.Size = new Size(0x36, 13);
            this.label11.TabIndex = 0x10;
            this.label11.Text = "patchersp";
            this.patchersp.Location = new Point(0x169, 150);
            this.patchersp.Name = "patchersp";
            this.patchersp.Size = new Size(60, 20);
            this.patchersp.TabIndex = 15;
            this.patchersp.Text = "027C1000";
            base.AutoScaleDimensions = new SizeF(6f, 13f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.ClientSize = new Size(530, 0xd3);
            base.Controls.Add(this.label11);
            base.Controls.Add(this.patchersp);
            base.Controls.Add(this.button6);
            base.Controls.Add(this.label9);
            base.Controls.Add(this.label8);
            base.Controls.Add(this.label6);
            base.Controls.Add(this.label7);
            base.Controls.Add(this.label3);
            base.Controls.Add(this.cv3);
            base.Controls.Add(this.readeduserspT);
            base.Controls.Add(this.button4);
            base.Controls.Add(this.readedirqspT);
            base.Controls.Add(this.userstack);
            base.Controls.Add(this.irqstack);
            base.Controls.Add(this.label5);
            base.Controls.Add(this.teilgr);
            base.Controls.Add(this.spwechseln);
            base.Controls.Add(this.label4);
            base.Controls.Add(this.label10);
            base.Controls.Add(this.label2);
            base.Controls.Add(this.label1);
            base.Controls.Add(this.button5);
            base.Controls.Add(this.button3);
            base.Controls.Add(this.cheatdatei);
            base.Controls.Add(this.button2);
            base.Controls.Add(this.ipspf);
            base.Controls.Add(this.button7);
            base.Controls.Add(this.button1);
            base.Controls.Add(this.HBfiletext);
            base.Controls.Add(this.gbapf);
            base.FormBorderStyle = FormBorderStyle.FixedToolWindow;
            base.Name = "Form1";
            this.Text = "ichflys_GBA_emu_patch_generator";
            base.ResumeLayout(false);
            base.PerformLayout();
        }

        public bool isMultilineWithData(int i)
        {
            if ((i < this.cheatsNumber) && (i >= 0))
            {
                switch (this.gbareadBYTE4_TO_UINT(this.cheatlist, (uint) (i * 0x1c)))
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                    case 0x10:
                    case 0x11:
                    case 0x12:
                    case 0x13:
                    case 20:
                    case 0x15:
                    case 0x16:
                    case 0x17:
                    case 0x18:
                    case 0x19:
                    case 0x1a:
                    case 0x1b:
                    case 0x1c:
                    case 0x1d:
                    case 30:
                    case 0x1f:
                    case 0x20:
                    case 0x21:
                    case 0x22:
                    case 0x23:
                    case 0x25:
                    case 0x26:
                    case 0x27:
                    case 40:
                    case 0x29:
                    case 0x2a:
                    case 0x2b:
                    case 0x2c:
                    case 0x2d:
                    case 0x2e:
                    case 0x2f:
                    case 0x30:
                    case 0x31:
                    case 50:
                    case 0x33:
                    case 0x34:
                    case 0x35:
                    case 0x36:
                    case 0x37:
                    case 0x38:
                    case 0x39:
                    case 0x3a:
                    case 0x3b:
                    case 60:
                    case 0x3d:
                    case 0x3e:
                    case 0x3f:
                    case 0x40:
                    case 0x41:
                    case 0x42:
                    case 0x43:
                    case 0x44:
                    case 0x45:
                    case 70:
                    case 0x47:
                    case 0x48:
                    case 0x49:
                    case 0x4a:
                    case 0x4b:
                    case 0x4c:
                    case 0x4d:
                    case 0x4e:
                    case 0x4f:
                    case 80:
                    case 0x51:
                    case 0x52:
                    case 0x53:
                    case 0x54:
                    case 0x55:
                    case 0x56:
                    case 0x57:
                    case 0x58:
                    case 0x59:
                    case 90:
                    case 0x5b:
                    case 0x5c:
                    case 0x5d:
                    case 0x5e:
                    case 0x5f:
                    case 0x60:
                    case 0x61:
                    case 0x62:
                    case 0x63:
                    case 100:
                    case 0x65:
                    case 0x66:
                    case 0x67:
                    case 0x68:
                    case 0x69:
                    case 0x6a:
                    case 0x6b:
                    case 0x6c:
                    case 0x6d:
                    case 110:
                    case 0x6f:
                    case 0x70:
                    case 0x72:
                    case 0x73:
                        return false;

                    case 9:
                    case 0x24:
                        return true;
                }
            }
            return false;
        }

        public void patchips(FileStream ipsfile, ref byte[] patchedfile)
        {
            while (true)
            {
                uint num = this.readBYTE3_TO_UINT(ref ipsfile);
                if (num == 0x454f46)
                {
                    return;
                }
                uint num2 = this.readBYTE2_TO_UINT(ref ipsfile);
                if (num2 == 0)
                {
                    uint num3 = this.readBYTE2_TO_UINT(ref ipsfile);
                    byte num4 = (byte) ipsfile.ReadByte();
                    for (uint i = num; i > (num + num3); i++)
                    {
                        patchedfile[i] = num4;
                    }
                }
                else
                {
                    ipsfile.Read(patchedfile, (int) num, (int) num2);
                }
            }
        }

        public void Patchsp(ref byte[] patchedfile)
        {
            uint offset = ((this.gbareadBYTE4_TO_UINT(patchedfile, 0) & 0xffffff) * 4) + 8;
            if (this.Comparrawchuck(offset, patchedfile, this.patcheins, this.patcheins.Length, this.patcheinsandplat))
            {
                if (this.userstack.Text != "")
                {
                    this.storu32((uint) Convert.ToInt32(this.userstack.Text, 0x10), ref patchedfile, offset + ((uint) this.patcheins.Length));
                }
                if (this.irqstack.Text != "")
                {
                    this.storu32((uint) Convert.ToInt32(this.irqstack.Text, 0x10), ref patchedfile, (uint) ((offset + this.patcheins.Length) + ((ulong) 4L)));
                }
            }
            else
            {
                MessageBox.Show("error patcheins nicht gefunden");
            }
        }

        public void Patchspsearch(ref byte[] patchedfile, ref uint irq, ref uint User)
        {
            uint offset = ((this.gbareadBYTE4_TO_UINT(patchedfile, 0) & 0xffffff) * 4) + 8;
            if (this.Comparrawchuck(offset, patchedfile, this.patcheins, this.patcheins.Length, this.patcheinsandplat))
            {
                User = this.gbareadBYTE4_TO_UINT(patchedfile, offset + ((uint) this.patcheins.Length));
                irq = this.gbareadBYTE4_TO_UINT(patchedfile, (uint) ((offset + this.patcheins.Length) + ((ulong) 4L)));
            }
            else
            {
                MessageBox.Show("error patcheins nicht gefunden");
            }
        }

        public uint readBYTE2_TO_UINT(ref FileStream str)
        {
            uint num = (uint) str.ReadByte();
            uint num2 = (uint) str.ReadByte();
            return (((num << 8) & 0xff00) | (num2 & 0xff));
        }

        public uint readBYTE3_TO_UINT(ref FileStream str)
        {
            uint num = (uint) str.ReadByte();
            uint num2 = (uint) str.ReadByte();
            uint num3 = (uint) str.ReadByte();
            return ((((num << 0x10) & 0xff0000) | ((num2 << 8) & 0xff00)) | (num3 & 0xff));
        }

        public void storu32(uint tostr, ref Stream str)
        {
            str.WriteByte((byte) tostr);
            str.WriteByte((byte) (tostr >> 8));
            str.WriteByte((byte) (tostr >> 0x10));
            str.WriteByte((byte) (tostr >> 0x18));
        }

        public void storu32(uint tostr, ref byte[] fild, uint offset)
        {
            fild[offset] = (byte) tostr;
            fild[(int) ((IntPtr) (offset + 1))] = (byte) (tostr >> 8);
            fild[(int) ((IntPtr) (offset + 2))] = (byte) (tostr >> 0x10);
            fild[(int) ((IntPtr) (offset + 3))] = (byte) (tostr >> 0x18);
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
        }

        public void writeHBpatch(ref int currentheaderoffset, ref int patchnumm, ref Stream ichflypatch)
        {
            patchnumm++;
            int position = (int) ichflypatch.Position;
            FileStream stream = new FileStream(this.HBfiletext.Text, FileMode.Open);
            byte[] buffer = new byte[stream.Length];
            stream.Read(buffer, 0, buffer.Length);
            ichflypatch.Write(buffer, 0, buffer.Length);
            int num2 = (int) ichflypatch.Position;
            ichflypatch.Position = (long) currentheaderoffset;
            this.storu32(5, ref ichflypatch);
            this.storu32((uint) position, ref ichflypatch);
            ichflypatch.Position = num2;
            currentheaderoffset += 8;
        }

        private void writeichflycheat(ref int currentheaderoffset, ref int patchnumm, ref Stream ichflypatch, int cheatsNumber, byte[] cheatlist)
        {
            patchnumm++;
            ichflypatch.Write(cheatlist, 0, cheatsNumber * 0x1c);
            int position = (int) ichflypatch.Position;
            ichflypatch.Position = (long) currentheaderoffset;
            this.storu32(1, ref ichflypatch);
            this.storu32((uint) cheatsNumber, ref ichflypatch);
            this.storu32((uint) (position - (cheatsNumber * 0x1c)), ref ichflypatch);
            ichflypatch.Position = position;
            currentheaderoffset += 12;
        }

        public bool writeichflypatch(ref int currentheaderoffset, ref int patchnumm, ref Stream ichflypatch, byte[] originalfile, byte[] patchedfile)
        {
            int offset = 0;
            int num2 = 0;
            while (offset != 0x2000000)
            {
                if (!this.Compararchuck((uint) offset, originalfile, patchedfile))
                {
                    if (num2 == patchnumm)
                    {
                        patchnumm++;
                        ichflypatch.Write(patchedfile, offset, 0x200 * (Convert.ToByte(this.teilgr.Text) - Convert.ToByte("0")));
                        int position = (int) ichflypatch.Position;
                        ichflypatch.Position = (long) currentheaderoffset;
                        this.storu32(0, ref ichflypatch);
                        this.storu32((uint) offset, ref ichflypatch);
                        this.storu32((uint) (position - (0x200 * (Convert.ToByte(this.teilgr.Text) - Convert.ToByte("0")))), ref ichflypatch);
                        ichflypatch.Position = position;
                        currentheaderoffset += 12;
                        return true;
                    }
                    num2++;
                }
                offset += (int) this.tempnum;
            }
            return false;
        }

        private void writeichflyuserpatches(ref int currentheaderoffset, ref int patchnumm, ref Stream ichflypatch)
        {
            patchnumm = (int) (patchnumm + this.userpatchsnum);
            int position = (int) ichflypatch.Position;
            ichflypatch.Position = (long) currentheaderoffset;
            this.storu32(2, ref ichflypatch);
            this.storu32(0x1ffc000, ref ichflypatch);
            this.storu32(0x4000, ref ichflypatch);
            this.storu32((uint) position, ref ichflypatch);
            ichflypatch.Write(this.patchbuff, 0, (int) (this.userpatchsnum * 0x11));
            ichflypatch.Position = position;
            currentheaderoffset = (int) (currentheaderoffset + (this.userpatchsnum * 13));
            ichflypatch.Write(this.codebuff, 0, 0x4000);
        }
    }
}

