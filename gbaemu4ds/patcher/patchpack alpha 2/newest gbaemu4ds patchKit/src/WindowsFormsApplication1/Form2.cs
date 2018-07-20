namespace WindowsFormsApplication1
{
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Windows.Forms;

    public class Form2 : Form
    {
        private testteil[] all;
        private CheckBox ARMbut;
        private CheckedListBox checkedListBox1;
        private byte[] codebuff;
        private IContainer components;
        private byte[] iram = new byte[0x8000];
        private Label label1;
        private int loadcurretn;
        private Form1 Mainform;
        private MaskedTextBox maskedTextBox1;
        private CheckBox only_card;
        private CheckBox only_IO;
        private CheckBox only_Video;
        private byte[] originalfile;
        private byte[] patchbuff;
        private byte[] patchedfile;
        private int[] possitions = new int[0x100000];
        private CheckBox THUMBbut;
        private uint userpatchsnum;
        private byte[] wram = new byte[0x40000];

        public Form2()
        {
            this.InitializeComponent();
        }

        private void checkedListBox1_MouseClick(object sender, MouseEventArgs e)
        {
            ListBox box = (ListBox) sender;
            Form3 form = new Form3();
            form.Show();
            form.init(ref this.originalfile, ref this.iram, ref this.wram, (uint) Convert.ToInt32(box.Text.Split(new char[] { ' ' })[0], 0x10), (uint) Convert.ToInt32(box.Text.Split(new char[] { ' ' })[1], 0x10), (uint) Convert.ToInt32(box.Text.Split(new char[] { ' ' })[2], 0x10), 12, ref this.originalfile, ref this.patchedfile, ref this.codebuff, ref this.patchbuff, ref this.userpatchsnum, this.Mainform);
        }

        private void checkedListBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
        }

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        public uint gbareadBYTE4_TO_UINT(ref FileStream str)
        {
            uint num = (uint) str.ReadByte();
            uint num2 = (uint) str.ReadByte();
            uint num3 = (uint) str.ReadByte();
            uint num4 = (uint) str.ReadByte();
            return ((((num & 0xff) | ((num4 << 0x18) & 0xff000000)) | ((num3 << 0x10) & 0xff0000)) | ((num2 << 8) & 0xff00));
        }

        private void InitializeComponent()
        {
            this.checkedListBox1 = new CheckedListBox();
            this.label1 = new Label();
            this.maskedTextBox1 = new MaskedTextBox();
            this.only_IO = new CheckBox();
            this.only_card = new CheckBox();
            this.only_Video = new CheckBox();
            this.ARMbut = new CheckBox();
            this.THUMBbut = new CheckBox();
            base.SuspendLayout();
            this.checkedListBox1.FormattingEnabled = true;
            this.checkedListBox1.Location = new Point(12, 0x1b);
            this.checkedListBox1.Name = "checkedListBox1";
            this.checkedListBox1.Size = new Size(0x253, 0x220);
            this.checkedListBox1.TabIndex = 0;
            this.checkedListBox1.MouseClick += new MouseEventHandler(this.checkedListBox1_MouseClick);
            this.checkedListBox1.SelectedIndexChanged += new EventHandler(this.checkedListBox1_SelectedIndexChanged);
            this.label1.AutoSize = true;
            this.label1.Location = new Point(9, 9);
            this.label1.Name = "label1";
            this.label1.Size = new Size(0x2b, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "max PC";
            this.maskedTextBox1.Location = new Point(0x3a, 6);
            this.maskedTextBox1.Name = "maskedTextBox1";
            this.maskedTextBox1.Size = new Size(100, 20);
            this.maskedTextBox1.TabIndex = 2;
            this.only_IO.AutoSize = true;
            this.only_IO.Checked = true;
            this.only_IO.CheckState = CheckState.Checked;
            this.only_IO.Location = new Point(0xa4, 8);
            this.only_IO.Name = "only_IO";
            this.only_IO.Size = new Size(0x25, 0x11);
            this.only_IO.TabIndex = 3;
            this.only_IO.Text = "IO";
            this.only_IO.UseVisualStyleBackColor = true;
            this.only_IO.CheckedChanged += new EventHandler(this.only_IO_CheckedChanged);
            this.only_card.AutoSize = true;
            this.only_card.Checked = true;
            this.only_card.CheckState = CheckState.Checked;
            this.only_card.Location = new Point(0xcf, 8);
            this.only_card.Name = "only_card";
            this.only_card.Size = new Size(0x33, 0x11);
            this.only_card.TabIndex = 3;
            this.only_card.Text = "Karte";
            this.only_card.UseVisualStyleBackColor = true;
            this.only_card.CheckedChanged += new EventHandler(this.only_IO_CheckedChanged);
            this.only_Video.AutoSize = true;
            this.only_Video.Location = new Point(0x108, 8);
            this.only_Video.Name = "only_Video";
            this.only_Video.Size = new Size(0x35, 0x11);
            this.only_Video.TabIndex = 3;
            this.only_Video.Text = "Video";
            this.only_Video.UseVisualStyleBackColor = true;
            this.only_Video.CheckedChanged += new EventHandler(this.only_IO_CheckedChanged);
            this.ARMbut.AutoSize = true;
            this.ARMbut.Checked = true;
            this.ARMbut.CheckState = CheckState.Checked;
            this.ARMbut.Location = new Point(0x155, 6);
            this.ARMbut.Name = "ARMbut";
            this.ARMbut.Size = new Size(50, 0x11);
            this.ARMbut.TabIndex = 3;
            this.ARMbut.Text = "ARM";
            this.ARMbut.UseVisualStyleBackColor = true;
            this.ARMbut.CheckedChanged += new EventHandler(this.only_IO_CheckedChanged);
            this.THUMBbut.AutoSize = true;
            this.THUMBbut.Location = new Point(400, 6);
            this.THUMBbut.Name = "THUMBbut";
            this.THUMBbut.Size = new Size(0x67, 0x11);
            this.THUMBbut.TabIndex = 3;
            this.THUMBbut.Text = "THUMB(not yet)";
            this.THUMBbut.UseVisualStyleBackColor = true;
            this.THUMBbut.CheckedChanged += new EventHandler(this.only_IO_CheckedChanged);
            base.AutoScaleDimensions = new SizeF(6f, 13f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.ClientSize = new Size(0x26b, 0x256);
            base.Controls.Add(this.THUMBbut);
            base.Controls.Add(this.ARMbut);
            base.Controls.Add(this.only_Video);
            base.Controls.Add(this.only_card);
            base.Controls.Add(this.only_IO);
            base.Controls.Add(this.maskedTextBox1);
            base.Controls.Add(this.label1);
            base.Controls.Add(this.checkedListBox1);
            base.Name = "Form2";
            this.Text = "dump Anzeiger";
            base.ResumeLayout(false);
            base.PerformLayout();
        }

        public void laodin(string gbafilepathname, string gbaemulatorresultpathname, ref byte[] originalfilea, ref byte[] patchedfilei, ref byte[] codebuffa, ref byte[] patchbuffa, ref uint userpatchsnuma, Form1 Mainforma)
        {
            this.originalfile = originalfilea;
            this.Mainform = Mainforma;
            this.userpatchsnum = userpatchsnuma;
            this.codebuff = codebuffa;
            this.patchbuff = patchbuffa;
            this.patchedfile = patchedfilei;
            FileStream str = new FileStream(gbaemulatorresultpathname, FileMode.Open);
            uint num = this.gbareadBYTE4_TO_UINT(ref str);
            uint num2 = this.gbareadBYTE4_TO_UINT(ref str);
            this.all = new testteil[num];
            this.maskedTextBox1.Text = Convert.ToString((long) num2, 0x10);
            for (int i = 0; i < num; i++)
            {
                this.readstate(ref str, num);
            }
            this.showall();
            str.Seek(-294912L, SeekOrigin.End);
            str.Read(this.wram, 0, 0x40000);
            str.Read(this.iram, 0, 0x8000);
            str.Close();
        }

        private void only_IO_CheckedChanged(object sender, EventArgs e)
        {
            this.showall();
        }

        public uint readBYTE4_TO_UINT(ref FileStream str)
        {
            uint num = (uint) str.ReadByte();
            uint num2 = (uint) str.ReadByte();
            uint num3 = (uint) str.ReadByte();
            return (((((uint) (str.ReadByte() & 0xff)) | ((num << 0x18) & 0xff000000)) | ((num2 << 0x10) & 0xff0000)) | ((num3 << 8) & 0xff00));
        }

        public void readstate(ref FileStream instr, uint num)
        {
            long position = instr.Position;
            this.all[this.loadcurretn].instaddr = this.gbareadBYTE4_TO_UINT(ref instr);
            instr.Seek((long) (num * 4), SeekOrigin.Current);
            this.all[this.loadcurretn].formaddr = this.gbareadBYTE4_TO_UINT(ref instr);
            instr.Seek((long) (num * 4), SeekOrigin.Current);
            this.all[this.loadcurretn].times = this.gbareadBYTE4_TO_UINT(ref instr);
            instr.Position = position + 4L;
            this.loadcurretn++;
        }

        public void showall()
        {
            this.checkedListBox1.Items.Clear();
            int index = 0;
            int num2 = 0x7fffffff;
            int times = 0;
            int num4 = 0x7fffffff;
            int num5 = 0;
            foreach (Form2.testteil testteil in this.all)
            {
                if (testteil.instaddr != 0)
                {
                    int num6 = 0;
                    foreach (Form2.testteil testteil2 in this.all)
                    {
                        if ((testteil2.times > times) && ((testteil2.times < num2) || ((num4 < num6) && (testteil2.times == num2))))
                        {
                            index = num6;
                            times = (int) testteil2.times;
                        }
                        num6++;
                    }
                    num4 = index;
                    num2 = times;
                    times = -1;
                    if (((((this.only_card.Checked && ((this.all[index].formaddr & 0x8000000) != 0)) || (((this.all[index].formaddr & 0xf000000) == 0x4000000) && this.only_IO.Checked)) || ((((this.all[index].formaddr & 0xf000000) != 0x4000000) && ((this.all[index].formaddr & 0x8000000) == 0)) && this.only_Video.Checked)) && ((((this.all[index].instaddr & 1) == 0) && this.ARMbut.Checked) || (((this.all[index].instaddr & 1) == 1) && this.THUMBbut.Checked))) && (this.all[index].instaddr > 0x1ffffff))
                    {
                        this.checkedListBox1.Items.Add(Convert.ToString((long) this.all[index].instaddr, 0x10) + " " + Convert.ToString((long) this.all[index].formaddr, 0x10) + " " + Convert.ToString((long) this.all[index].times, 10), false);
                    }
                    this.possitions[num5] = index;
                    num5++;
                }
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct testteil
        {
            public uint instaddr;
            public uint formaddr;
            public uint times;
        }
    }
}

