namespace WindowsFormsApplication1
{
    using System;
    using System.ComponentModel;
    using System.Drawing;
    using System.Windows.Forms;

    public class Form3 : Form
    {
        private Button button1;
        private Button button2;
        private CheckBox checkBox2;
        private byte[] codebuff;
        private IContainer components;
        private CheckBox Droc;
        private GroupBox groupBox1;
        private GroupBox groupBox2;
        private GroupBox groupBox3;
        private uint instraddrint;
        private ListBox listBox1;
        private Form1 Mainform;
        private CheckBox noirq;
        private CheckBox onlyPUemulation;
        private byte[] originalfile;
        private byte[] patchbuff;
        private byte[] patchedfile;
        private CheckBox pugba;
        private CheckBox punds;
        private CheckBox saveCPSR;
        private CheckBox spchange;
        private RadioButton UR10;
        private RadioButton UR11;
        private RadioButton UR12;
        private RadioButton UR13;
        private RadioButton UR14;
        private RadioButton UR15;
        private RadioButton UR16;
        private RadioButton UR17;
        private RadioButton UR18;
        private RadioButton UR19;
        private RadioButton UR1A;
        private RadioButton UR1B;
        private RadioButton UR1C;
        private RadioButton UR1D;
        private RadioButton UR1E;
        private RadioButton UR20;
        private RadioButton UR21;
        private RadioButton UR22;
        private RadioButton UR23;
        private RadioButton UR24;
        private RadioButton UR25;
        private RadioButton UR26;
        private RadioButton UR27;
        private RadioButton UR28;
        private RadioButton UR29;
        private RadioButton UR2A;
        private RadioButton UR2B;
        private RadioButton UR2C;
        private RadioButton UR2D;
        private RadioButton UR2E;
        private uint userpatchsnum;
        private CheckBox Z0;
        private CheckBox Z1;
        private CheckBox Z12;
        private CheckBox Z13;
        private CheckBox Z14;
        private CheckBox Z2;
        private CheckBox Z3;
        private CheckBox ZU1;
        private CheckBox ZU2;

        public Form3()
        {
            this.InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (this.listBox1.SelectedItem == null)
            {
                MessageBox.Show("error");
            }
            else
            {
                uint num = (uint) Convert.ToInt32(this.listBox1.SelectedItem.ToString(), 0x10);
                this.Mainform.userpatchsnumstat++;
                uint instaddr = 0x2000000 - (this.Mainform.userpatchsnumstat * 0x100);
                uint num3 = 0x3fff;
                this.storu32(((this.gbareadBYTE4_TO_UINT(this.patchedfile, num & 0x1ffffff) & 0xf0000000) | 0xa000000) | ((((instaddr - 8) - this.instraddrint) / 4) & 0xffffff), ref this.patchedfile, num & 0x1ffffff);
                if ((!this.saveCPSR.Checked && !this.noirq.Checked) && !this.spchange.Checked)
                {
                    if (((!this.Z0.Checked || !this.Z1.Checked) || (!this.Z2.Checked || !this.Z3.Checked)) || ((!this.Z12.Checked || !this.Z13.Checked) || ((!this.Z14.Checked || !this.ZU1.Checked) || !this.ZU2.Checked)))
                    {
                        this.storu32(this.generatestmfd((ushort) (this.getfirststmfdbitmap() | this.getsecstmfdbitmap()), 13), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                }
                else
                {
                    if (!this.ZU1.Checked || !this.ZU2.Checked)
                    {
                        this.storu32(this.generatestmfd(this.getfirststmfdbitmap(), 13), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.saveCPSR.Checked || this.noirq.Checked)
                    {
                        this.storu32(this.generatmrscpsr(this.getU1()), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.noirq.Checked)
                    {
                        this.storu32(this.generatorri(this.getU2(), this.getU1(), 0x80, 0), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.noirq.Checked)
                    {
                        this.storu32(this.generatmsrcpsr(this.getU2()), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.spchange.Checked)
                    {
                        this.storu32(this.generateldr32Bit(this.Mainform.sppatcher, this.getU2(), (instaddr & ((uint) 18446744073709551360L)) + 240, instaddr), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (((!this.Z0.Checked || !this.Z1.Checked) || (!this.Z2.Checked || !this.Z3.Checked)) || ((!this.Z12.Checked || !this.Z13.Checked) || !this.Z14.Checked))
                    {
                        if (this.spchange.Checked)
                        {
                            this.storu32(this.generatestmfd(this.getsecstmfdbitmap(), this.getU2()), ref this.codebuff, num3 & instaddr);
                        }
                        else
                        {
                            this.storu32(this.generatestmfd(this.getsecstmfdbitmap(), 13), ref this.codebuff, num3 & instaddr);
                        }
                        instaddr += 4;
                    }
                }
                if (this.need_swap(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)))
                {
                    if (this.generatecpymoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)) != uint.MaxValue)
                    {
                        this.storu32(this.generatecpymoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.generatefirstmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)) != uint.MaxValue)
                    {
                        this.storu32(this.generatefirstmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                }
                else
                {
                    if (this.generatefirstmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)) != uint.MaxValue)
                    {
                        this.storu32(this.generatefirstmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.generatecpymoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)) != uint.MaxValue)
                    {
                        this.storu32(this.generatecpymoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                }
                if (this.spchange.Checked)
                {
                    this.storu32(this.generatorri(13, this.getU2(), 0, 0), ref this.codebuff, num3 & instaddr);
                    instaddr += 4;
                }
                if (this.punds.Checked)
                {
                    this.storu32(this.generateldr32Bit(0x36333333, this.getU2(), (instaddr & ((uint) 18446744073709551360L)) + 0xf4, instaddr), ref this.codebuff, num3 & instaddr);
                    instaddr += 4;
                }
                if (this.punds.Checked)
                {
                    this.storu32(this.generatmcr(0, 5, this.getU2(), 15, 2, 0), ref this.codebuff, num3 & instaddr);
                    instaddr += 4;
                }
                if (!this.Droc.Checked)
                {
                    this.storu32(4, ref this.patchbuff, this.Mainform.userpatchsnum * 0x11);
                    this.patchbuff[(int) ((IntPtr) ((this.Mainform.userpatchsnum * 0x11) + 4))] = 3;
                    this.storu32(instaddr, ref this.patchbuff, (this.Mainform.userpatchsnum * 0x11) + 5);
                    this.storu32(this.getinsttype(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.patchbuff, (this.Mainform.userpatchsnum * 0x11) + 9);
                    this.storu32(14, ref this.patchbuff, ((this.Mainform.userpatchsnum * 0x11) + 9) + 4);
                    this.Mainform.userpatchsnum++;
                }
                else
                {
                    this.storu32(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff), ref this.codebuff, num3 & instaddr);
                }
                this.Mainform.douserpatchs = true;
                instaddr += 4;
                if ((this.generatelastmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)) != uint.MaxValue) && (((!this.Z0.Checked || !this.Z1.Checked) || (!this.Z2.Checked || !this.Z3.Checked)) || ((!this.Z12.Checked || !this.Z13.Checked) || !this.Z14.Checked)))
                {
                    this.storu32(this.generatelastmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.codebuff, num3 & instaddr);
                    instaddr += 4;
                }
                if (this.pugba.Checked)
                {
                    if (this.punds.Checked)
                    {
                        this.storu32(this.generatbici(this.getU2(), this.getU2(), 0x30, 4), ref this.codebuff, num3 & instaddr);
                    }
                    else
                    {
                        this.storu32(this.generateldr32Bit(0x6333333, this.getU2(), (instaddr & ((uint) 18446744073709551360L)) + 0xf4, instaddr), ref this.codebuff, num3 & instaddr);
                    }
                    instaddr += 4;
                }
                if (this.pugba.Checked)
                {
                    this.storu32(this.generatmcr(0, 5, this.getU2(), 15, 2, 0), ref this.codebuff, num3 & instaddr);
                    instaddr += 4;
                }
                if ((!this.saveCPSR.Checked && !this.noirq.Checked) && !this.spchange.Checked)
                {
                    if (((!this.Z0.Checked || !this.Z1.Checked) || (!this.Z2.Checked || !this.Z3.Checked)) || ((!this.Z12.Checked || !this.Z13.Checked) || ((!this.Z14.Checked || !this.ZU1.Checked) || !this.ZU2.Checked)))
                    {
                        this.storu32(this.generateldmfd((ushort) (this.getfirststmfdbitmap() | this.getsecstmfdbitmap()), 13), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                }
                else
                {
                    if (((!this.Z0.Checked || !this.Z1.Checked) || (!this.Z2.Checked || !this.Z3.Checked)) || ((!this.Z12.Checked || !this.Z13.Checked) || !this.Z14.Checked))
                    {
                        this.storu32(this.generateldmfd(this.getsecstmfdbitmap(), 13), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (this.saveCPSR.Checked)
                    {
                        this.storu32(this.generatmsrcpsr(this.getU1()), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                    if (!this.ZU1.Checked || !this.ZU2.Checked)
                    {
                        this.storu32(this.generateldmfd(this.getfirststmfdbitmap(), 13), ref this.codebuff, num3 & instaddr);
                        instaddr += 4;
                    }
                }
                if (this.generatesecmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)) != uint.MaxValue)
                {
                    this.storu32(this.generatesecmoded(this.gbareadBYTE4_TO_UINT(this.originalfile, num & 0x1ffffff)), ref this.codebuff, num3 & instaddr);
                    instaddr += 4;
                }
                this.storu32(this.generateB((this.instraddrint - instaddr) - 4), ref this.codebuff, num3 & instaddr);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            base.Close();
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

        protected override void Dispose(bool disposing)
        {
            if (disposing && (this.components != null))
            {
                this.components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void Droc_CheckedChanged(object sender, EventArgs e)
        {
            if (this.Droc.Checked)
            {
                this.Z0.Checked = true;
                this.Z0.Enabled = false;
                this.Z1.Checked = true;
                this.Z1.Enabled = false;
                this.Z2.Checked = true;
                this.Z2.Enabled = false;
                this.Z3.Checked = true;
                this.Z3.Enabled = false;
                this.Z12.Checked = true;
                this.Z12.Enabled = false;
                this.Z13.Checked = true;
                this.Z13.Enabled = false;
                this.Z14.Checked = true;
                this.Z14.Enabled = false;
                this.ZU1.Checked = true;
                this.ZU1.Enabled = false;
                this.saveCPSR.Checked = false;
                this.saveCPSR.Enabled = false;
                this.noirq.Checked = false;
                this.noirq.Enabled = false;
                this.spchange.Checked = false;
                this.spchange.Enabled = false;
                this.checkBox2.Checked = false;
                this.checkBox2.Enabled = false;
                this.onlyPUemulation.Enabled = false;
                this.groupBox2.Enabled = false;
            }
            else
            {
                this.Z0.Enabled = true;
                this.Z1.Enabled = true;
                this.Z2.Enabled = true;
                this.Z3.Enabled = true;
                this.Z12.Enabled = true;
                this.Z13.Enabled = true;
                this.Z14.Enabled = true;
                this.ZU1.Enabled = true;
                this.saveCPSR.Enabled = true;
                this.noirq.Enabled = true;
                this.spchange.Enabled = true;
                this.checkBox2.Enabled = true;
                this.onlyPUemulation.Enabled = true;
            }
        }

        public uint gbareadBYTE4_TO_UINT(byte[] fild, uint offset)
        {
            uint num = fild[offset];
            uint num2 = fild[(int) ((IntPtr) (offset + 1))];
            uint num3 = fild[(int) ((IntPtr) (offset + 2))];
            uint num4 = fild[(int) ((IntPtr) (offset + 3))];
            return (((((num4 << 0x18) & 0xff000000) | ((num3 << 0x10) & 0xff0000)) | ((num2 << 8) & 0xff00)) | (num & 0xff));
        }

        private uint generatbici(byte target, byte src1, byte Byte, byte seek)
        {
            return (uint) ((((0xe3c00000L | (src1 << 0x10)) | (target << 12)) | (seek << 8)) | Byte);
        }

        private uint generateB(uint offset)
        {
            return (0xea000000 | (offset >> 2));
        }

        private uint generatecpymoded(uint instr)
        {
            if (!this.Droc.Checked)
            {
                uint num = 0;
                bool flag = false;
                if ((((instr & 0xe000000) == 0) && ((instr & 0x100000) != 0x100000)) && ((instr & 0x60) == 0x20))
                {
                    flag = true;
                }
                if (((instr & 0xc000000) == 0x4000000) && ((instr & 0x100000) != 0x100000))
                {
                    flag = true;
                }
                if (!flag)
                {
                    return uint.MaxValue;
                }
                if ((instr & 0xf000) != 0x1000)
                {
                    num = 0x1a01000;
                    num |= (uint) ((instr & 0xf000) >> 12);
                    return (0xe0000000 | num);
                }
            }
            return uint.MaxValue;
        }

        private uint generatefirstmoded(uint instr)
        {
            if (!this.Droc.Checked)
            {
                instr &= 0xfffffff;
                if ((instr & 0xe000000) == 0)
                {
                    uint num = 0;
                    if ((instr & 0x1000000) == 0x1000000)
                    {
                        if ((instr & 0x800000) == 0x800000)
                        {
                            num = 0x800000;
                        }
                        else
                        {
                            num = 0x400000;
                        }
                        num |= instr & 0xf0000;
                        if ((instr & 0x400000) == 0x400000)
                        {
                            num |= 0x2000000;
                            num |= instr & 15;
                            num |= (uint) ((instr & 0xf00) >> 4);
                        }
                        else
                        {
                            num |= instr & 15;
                        }
                    }
                    else
                    {
                        num = 0x1a00000;
                        num |= (uint) ((instr & 0xf0000) >> 0x10);
                    }
                    return (0xe0000000 | num);
                }
                if ((instr & 0xc000000) == 0x4000000)
                {
                    uint num2 = 0;
                    if ((instr & 0x1000000) == 0x1000000)
                    {
                        if ((instr & 0x800000) == 0x800000)
                        {
                            num2 = 0x800000;
                        }
                        else
                        {
                            num2 = 0x400000;
                        }
                        num2 |= instr & 0xf0000;
                        if ((instr & 0x2000000) != 0x2000000)
                        {
                            num2 |= 0x2000000;
                            if ((instr & 0xfff) < 0x100)
                            {
                                num2 |= instr & 0xff;
                            }
                            else
                            {
                                MessageBox.Show("todo great Single Data Transfer " + instr.ToString());
                            }
                        }
                        else
                        {
                            MessageBox.Show("todo great Single Data Transfer Register shifted by Immediate as Offset" + instr.ToString());
                        }
                    }
                    else
                    {
                        num2 = 0x1a00000;
                        num2 |= (uint) ((instr & 0xf0000) >> 0x10);
                    }
                    return (0xe0000000 | num2);
                }
                MessageBox.Show("Unknown OP " + instr.ToString());
            }
            return uint.MaxValue;
        }

        private uint generatelastmoded(uint instr)
        {
            if (!this.Droc.Checked)
            {
                instr &= 0xfffffff;
                if (((instr & 0xe000000) == 0) || ((instr & 0xc000000) == 0x4000000))
                {
                    uint num = 0;
                    if ((instr & 0x100000) == 0x100000)
                    {
                        num = 0x1a00000;
                        num |= instr & 0xf000;
                        return (0xe0000000 | num);
                    }
                    return uint.MaxValue;
                }
                MessageBox.Show("Unknown OP " + instr.ToString());
            }
            return uint.MaxValue;
        }

        private uint generateldmfd(ushort bitmap, byte target)
        {
            return (uint) ((0xe8b00000L | (target << 0x10)) | bitmap);
        }

        private uint generateldr32Bit(uint val, byte dest, uint nextfree, uint instaddr)
        {
            this.storu32(val, ref this.codebuff, nextfree & 0x3fff);
            return this.generatldr(dest, 15, (ushort) ((nextfree - 8) - instaddr));
        }

        private uint generatesecmoded(uint instr)
        {
            if (!this.Droc.Checked)
            {
                uint num = 0;
                if ((instr & 0xe000000) == 0)
                {
                    if (((instr & 0x1000000) != 0) && ((instr & 0x200000) != 0x200000))
                    {
                        return uint.MaxValue;
                    }
                    if ((instr & 0x800000) == 0x800000)
                    {
                        num = 0x800000;
                    }
                    else
                    {
                        num = 0x400000;
                    }
                    num |= instr & 0xf0000;
                    num |= (uint) ((instr & 0xf0000) >> 4);
                    if ((instr & 0x400000) == 0x400000)
                    {
                        num |= 0x2000000;
                        num |= instr & 15;
                        num |= (uint) ((instr & 0xf00) >> 4);
                    }
                    else
                    {
                        num |= instr & 15;
                    }
                    return (0xe0000000 | num);
                }
                if ((instr & 0xc000000) == 0x4000000)
                {
                    if (((instr & 0x1000000) == 0) || ((instr & 0x200000) == 0x200000))
                    {
                        if ((instr & 0x800000) == 0x800000)
                        {
                            num = 0x800000;
                        }
                        else
                        {
                            num = 0x400000;
                        }
                        num |= instr & 0xf0000;
                        num |= (uint) ((instr & 0xf0000) >> 4);
                        if ((instr & 0x2000000) != 0x2000000)
                        {
                            if ((instr & 0xfff) < 0x100)
                            {
                                num |= instr & 0xff;
                                return (0xe0000000 | num);
                            }
                            MessageBox.Show("todo great Single Data Transfer " + instr.ToString());
                            return uint.MaxValue;
                        }
                        MessageBox.Show("todo Single Data Transfer Register shifted by Immediate as Offset" + instr.ToString());
                    }
                    return uint.MaxValue;
                }
                MessageBox.Show("Unknown OP " + instr.ToString());
            }
            return uint.MaxValue;
        }

        private uint generatestmfd(ushort bitmap, byte target)
        {
            return (uint) ((0xe9200000L | (target << 0x10)) | bitmap);
        }

        private uint generatldr(byte srcp, byte target, ushort offset)
        {
            return (uint) ((((ulong) (-443547648 | offset)) | (srcp << 12)) | (target << 0x10));
        }

        private uint generatldri(byte srcp, byte target, ushort offset)
        {
            return (uint) ((((ulong) (-409993216 | offset)) | (srcp << 12)) | (target << 0x10));
        }

        private uint generatmcr(byte CP_Opc, byte Cn, byte Rd, byte Pn, byte CP, byte Cm)
        {
            return (uint) (((((((ulong) (-301989872 | Cm)) | (CP << 5)) | (Pn << 8)) | (Rd << 12)) | (Cn << 0x10)) | (CP_Opc << 0x15));
        }

        private uint generatmrscpsr(byte target)
        {
            return (uint) (0xe10f0000L | (target << 12));
        }

        private uint generatmsrcpsr(byte src)
        {
            return (uint) (-516952064 | src);
        }

        private uint generatorri(byte target, byte src1, byte Byte, byte seek)
        {
            return (uint) ((((0xe3800000L | (src1 << 0x10)) | (target << 12)) | (seek << 8)) | Byte);
        }

        private ushort getfirststmfdbitmap()
        {
            int num = 0;
            if (!this.ZU1.Checked)
            {
                num = ((int) 1) << this.getU1();
            }
            if (!this.ZU2.Checked)
            {
                num |= ((int) 1) << this.getU2();
            }
            return (ushort) num;
        }

        private uint getinsttype(uint instr)
        {
            uint num = 0;
            if ((instr & 0xe000000) == 0)
            {
                if ((instr & 0x100000) != 0x100000)
                {
                    num += 3;
                    if ((instr & 0x60) == 0x20)
                    {
                        num++;
                    }
                    else
                    {
                        MessageBox.Show("todo uncommon ldr/str " + instr.ToString());
                    }
                }
                else
                {
                    if (((instr & 0x60) == 0x40) & !this.onlyPUemulation.Checked)
                    {
                        return 13;
                    }
                    if (((instr & 0x60) == 0x40) & this.onlyPUemulation.Checked)
                    {
                        return 15;
                    }
                    if (((instr & 0x60) == 0x60) & !this.onlyPUemulation.Checked)
                    {
                        return 12;
                    }
                    if (((instr & 0x60) == 0x60) & this.onlyPUemulation.Checked)
                    {
                        return 14;
                    }
                    if ((instr & 0x60) == 0x20)
                    {
                        num++;
                    }
                    else
                    {
                        MessageBox.Show("todo uncommon ldr/str " + instr.ToString());
                    }
                }
            }
            if ((instr & 0xc000000) == 0x4000000)
            {
                if ((instr & 0x400000) == 0x400000)
                {
                    num += 2;
                }
                if ((instr & 0x100000) != 0x100000)
                {
                    num += 3;
                }
            }
            if (this.checkBox2.Checked)
            {
                return (num + 100);
            }
            if (!this.onlyPUemulation.Checked)
            {
                return (num + 6);
            }
            return num;
        }

        private ushort getsecstmfdbitmap()
        {
            int num = 0;
            if (!this.Z0.Checked)
            {
                num = 1;
            }
            if (!this.Z1.Checked)
            {
                num |= 2;
            }
            if (!this.Z2.Checked)
            {
                num |= 4;
            }
            if (!this.Z3.Checked)
            {
                num |= 8;
            }
            if (!this.Z12.Checked)
            {
                num |= 0x1000;
            }
            if (!this.Z13.Checked)
            {
                num |= 0x2000;
            }
            if (!this.Z14.Checked)
            {
                num |= 0x4000;
            }
            return (ushort) num;
        }

        private byte getU1()
        {
            if (this.UR10.Checked)
            {
                return 0;
            }
            if (this.UR11.Checked)
            {
                return 1;
            }
            if (this.UR12.Checked)
            {
                return 2;
            }
            if (this.UR13.Checked)
            {
                return 3;
            }
            if (this.UR14.Checked)
            {
                return 4;
            }
            if (this.UR15.Checked)
            {
                return 5;
            }
            if (this.UR16.Checked)
            {
                return 6;
            }
            if (this.UR17.Checked)
            {
                return 7;
            }
            if (this.UR18.Checked)
            {
                return 8;
            }
            if (this.UR19.Checked)
            {
                return 9;
            }
            if (!this.UR1A.Checked)
            {
                if (this.UR1B.Checked)
                {
                    return 11;
                }
                if (this.UR1C.Checked)
                {
                    return 12;
                }
                if (this.UR1D.Checked)
                {
                    return 13;
                }
                if (this.UR1E.Checked)
                {
                    return 14;
                }
            }
            return 10;
        }

        private byte getU2()
        {
            if (this.UR20.Checked)
            {
                return 0;
            }
            if (this.UR21.Checked)
            {
                return 1;
            }
            if (this.UR22.Checked)
            {
                return 2;
            }
            if (this.UR23.Checked)
            {
                return 3;
            }
            if (this.UR24.Checked)
            {
                return 4;
            }
            if (this.UR25.Checked)
            {
                return 5;
            }
            if (this.UR26.Checked)
            {
                return 6;
            }
            if (this.UR27.Checked)
            {
                return 7;
            }
            if (this.UR28.Checked)
            {
                return 8;
            }
            if (this.UR29.Checked)
            {
                return 9;
            }
            if (this.UR2A.Checked)
            {
                return 10;
            }
            if (!this.UR2B.Checked)
            {
                if (this.UR2C.Checked)
                {
                    return 12;
                }
                if (this.UR2D.Checked)
                {
                    return 13;
                }
                if (this.UR2E.Checked)
                {
                    return 14;
                }
            }
            return 11;
        }

        public void init(ref byte[] rom, ref byte[] iwram, ref byte[] wram, uint instraddr, uint dest, uint times, uint timescoder, ref byte[] originalfilea, ref byte[] patchedfilea, ref byte[] codebuffa, ref byte[] patchbuffa, ref uint userpatchsnuma, Form1 Mainforma)
        {
            this.Mainform = Mainforma;
            this.userpatchsnum = userpatchsnuma;
            this.codebuff = codebuffa;
            this.patchbuff = patchbuffa;
            this.originalfile = originalfilea;
            this.patchedfile = patchedfilea;
            this.instraddrint = instraddr;
            if ((instraddr & 0xff000000) == 0x3000000)
            {
                byte[] destinationArray = new byte[12];
                Array.Copy(iwram, ((int) instraddr) - 0x3000004, destinationArray, 0, 12);
                for (uint i = 0; i < (rom.Length - 12); i++)
                {
                    if (this.Comparrawchuck(i, rom, destinationArray, 12))
                    {
                        this.listBox1.Items.Add(Convert.ToString((long) ((i + 0x8000000) + 4), 0x10));
                    }
                }
            }
            else if ((instraddr & 0xff000000) == 0x2000000)
            {
                byte[] buffer2 = new byte[12];
                Array.Copy(wram, ((int) instraddr) - 0x2000000, buffer2, 0, 12);
                for (uint j = 0; j < (rom.Length - 12); j++)
                {
                    if (this.Comparrawchuck(j, rom, buffer2, 12))
                    {
                        this.listBox1.Items.Add(Convert.ToString((long) ((j + 0x8000000) + 4), 0x10));
                    }
                }
            }
            else
            {
                this.listBox1.Items.Add(Convert.ToString((long) instraddr, 0x10));
            }
        }

        private void InitializeComponent()
        {
            this.listBox1 = new ListBox();
            this.button1 = new Button();
            this.button2 = new Button();
            this.saveCPSR = new CheckBox();
            this.noirq = new CheckBox();
            this.spchange = new CheckBox();
            this.Z0 = new CheckBox();
            this.Z1 = new CheckBox();
            this.Z2 = new CheckBox();
            this.Z3 = new CheckBox();
            this.Z12 = new CheckBox();
            this.Z14 = new CheckBox();
            this.ZU1 = new CheckBox();
            this.ZU2 = new CheckBox();
            this.Z13 = new CheckBox();
            this.groupBox1 = new GroupBox();
            this.punds = new CheckBox();
            this.pugba = new CheckBox();
            this.groupBox2 = new GroupBox();
            this.UR18 = new RadioButton();
            this.UR17 = new RadioButton();
            this.UR16 = new RadioButton();
            this.UR15 = new RadioButton();
            this.UR14 = new RadioButton();
            this.UR13 = new RadioButton();
            this.UR12 = new RadioButton();
            this.UR11 = new RadioButton();
            this.UR1E = new RadioButton();
            this.UR1D = new RadioButton();
            this.UR1C = new RadioButton();
            this.UR1B = new RadioButton();
            this.UR1A = new RadioButton();
            this.UR19 = new RadioButton();
            this.UR10 = new RadioButton();
            this.groupBox3 = new GroupBox();
            this.UR28 = new RadioButton();
            this.UR27 = new RadioButton();
            this.UR26 = new RadioButton();
            this.UR25 = new RadioButton();
            this.UR24 = new RadioButton();
            this.UR23 = new RadioButton();
            this.UR22 = new RadioButton();
            this.UR21 = new RadioButton();
            this.UR2E = new RadioButton();
            this.UR2D = new RadioButton();
            this.UR2C = new RadioButton();
            this.UR2B = new RadioButton();
            this.UR2A = new RadioButton();
            this.UR29 = new RadioButton();
            this.UR20 = new RadioButton();
            this.onlyPUemulation = new CheckBox();
            this.checkBox2 = new CheckBox();
            this.Droc = new CheckBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            base.SuspendLayout();
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new Point(12, 0x24);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new Size(260, 0xba);
            this.listBox1.TabIndex = 2;
            this.button1.Location = new Point(12, 0xe4);
            this.button1.Name = "button1";
            this.button1.Size = new Size(130, 0x17);
            this.button1.TabIndex = 3;
            this.button1.Text = "Anwenden";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new EventHandler(this.button1_Click);
            this.button2.Location = new Point(0x94, 0xe3);
            this.button2.Name = "button2";
            this.button2.Size = new Size(0x7c, 0x17);
            this.button2.TabIndex = 3;
            this.button2.Text = "Close";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new EventHandler(this.button2_Click);
            this.saveCPSR.AutoSize = true;
            this.saveCPSR.Location = new Point(0x419, 0x21);
            this.saveCPSR.Name = "saveCPSR";
            this.saveCPSR.Size = new Size(0x5c, 0x11);
            this.saveCPSR.TabIndex = 4;
            this.saveCPSR.Text = "sichere CPSR";
            this.saveCPSR.UseVisualStyleBackColor = true;
            this.noirq.AutoSize = true;
            this.noirq.Location = new Point(0x419, 0x38);
            this.noirq.Name = "noirq";
            this.noirq.Size = new Size(0x44, 0x11);
            this.noirq.TabIndex = 4;
            this.noirq.Text = "kein IRQ";
            this.noirq.UseVisualStyleBackColor = true;
            this.spchange.AutoSize = true;
            this.spchange.Location = new Point(0x419, 0x4f);
            this.spchange.Name = "spchange";
            this.spchange.Size = new Size(0x5e, 0x11);
            this.spchange.TabIndex = 4;
            this.spchange.Text = "wechsle stack";
            this.spchange.UseVisualStyleBackColor = true;
            this.spchange.CheckedChanged += new EventHandler(this.spchange_CheckedChanged);
            this.Z0.AutoSize = true;
            this.Z0.Location = new Point(6, 0x17);
            this.Z0.Name = "Z0";
            this.Z0.Size = new Size(40, 0x11);
            this.Z0.TabIndex = 0;
            this.Z0.Text = "R0";
            this.Z0.UseVisualStyleBackColor = true;
            this.Z1.AutoSize = true;
            this.Z1.Location = new Point(6, 0x2e);
            this.Z1.Name = "Z1";
            this.Z1.Size = new Size(40, 0x11);
            this.Z1.TabIndex = 0;
            this.Z1.Text = "R1";
            this.Z1.UseVisualStyleBackColor = true;
            this.Z2.AutoSize = true;
            this.Z2.Location = new Point(6, 0x45);
            this.Z2.Name = "Z2";
            this.Z2.Size = new Size(40, 0x11);
            this.Z2.TabIndex = 0;
            this.Z2.Text = "R2";
            this.Z2.UseVisualStyleBackColor = true;
            this.Z3.AutoSize = true;
            this.Z3.Location = new Point(6, 0x5c);
            this.Z3.Name = "Z3";
            this.Z3.Size = new Size(40, 0x11);
            this.Z3.TabIndex = 0;
            this.Z3.Text = "R3";
            this.Z3.UseVisualStyleBackColor = true;
            this.Z12.AutoSize = true;
            this.Z12.Location = new Point(6, 0x73);
            this.Z12.Name = "Z12";
            this.Z12.Size = new Size(0x2e, 0x11);
            this.Z12.TabIndex = 0;
            this.Z12.Text = "R12";
            this.Z12.UseVisualStyleBackColor = true;
            this.Z14.AutoSize = true;
            this.Z14.Location = new Point(6, 0xa1);
            this.Z14.Name = "Z14";
            this.Z14.Size = new Size(0x45, 0x11);
            this.Z14.TabIndex = 0;
            this.Z14.Text = "LR (R14)";
            this.Z14.UseVisualStyleBackColor = true;
            this.ZU1.AutoSize = true;
            this.ZU1.Location = new Point(6, 0xb8);
            this.ZU1.Name = "ZU1";
            this.ZU1.Size = new Size(0x44, 0x11);
            this.ZU1.TabIndex = 0;
            this.ZU1.Text = "Used R1";
            this.ZU1.UseVisualStyleBackColor = true;
            this.ZU2.AutoSize = true;
            this.ZU2.Location = new Point(6, 0xcf);
            this.ZU2.Name = "ZU2";
            this.ZU2.Size = new Size(0x44, 0x11);
            this.ZU2.TabIndex = 0;
            this.ZU2.Text = "Used R2";
            this.ZU2.UseVisualStyleBackColor = true;
            this.Z13.AutoSize = true;
            this.Z13.Checked = true;
            this.Z13.CheckState = CheckState.Checked;
            this.Z13.Enabled = false;
            this.Z13.Location = new Point(6, 0x8a);
            this.Z13.Name = "Z13";
            this.Z13.Size = new Size(0x45, 0x11);
            this.Z13.TabIndex = 0;
            this.Z13.Text = "SP (R13)";
            this.Z13.UseVisualStyleBackColor = true;
            this.groupBox1.Controls.Add(this.Z13);
            this.groupBox1.Controls.Add(this.ZU2);
            this.groupBox1.Controls.Add(this.ZU1);
            this.groupBox1.Controls.Add(this.Z14);
            this.groupBox1.Controls.Add(this.Z12);
            this.groupBox1.Controls.Add(this.Z3);
            this.groupBox1.Controls.Add(this.Z2);
            this.groupBox1.Controls.Add(this.Z1);
            this.groupBox1.Controls.Add(this.Z0);
            this.groupBox1.Location = new Point(0x116, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new Size(0x7a, 0xed);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Zerst\x00f6rbare Register";
            this.punds.AutoSize = true;
            this.punds.Location = new Point(0x404, 0x66);
            this.punds.Name = "punds";
            this.punds.Size = new Size(0x73, 0x11);
            this.punds.TabIndex = 4;
            this.punds.Text = "setze pu nds mode";
            this.punds.UseVisualStyleBackColor = true;
            this.pugba.AutoSize = true;
            this.pugba.Location = new Point(0x404, 0x7d);
            this.pugba.Name = "pugba";
            this.pugba.Size = new Size(0x74, 0x11);
            this.pugba.TabIndex = 4;
            this.pugba.Text = "setze pu gba mode";
            this.pugba.UseVisualStyleBackColor = true;
            this.groupBox2.Controls.Add(this.UR18);
            this.groupBox2.Controls.Add(this.UR17);
            this.groupBox2.Controls.Add(this.UR16);
            this.groupBox2.Controls.Add(this.UR15);
            this.groupBox2.Controls.Add(this.UR14);
            this.groupBox2.Controls.Add(this.UR13);
            this.groupBox2.Controls.Add(this.UR12);
            this.groupBox2.Controls.Add(this.UR11);
            this.groupBox2.Controls.Add(this.UR1E);
            this.groupBox2.Controls.Add(this.UR1D);
            this.groupBox2.Controls.Add(this.UR1C);
            this.groupBox2.Controls.Add(this.UR1B);
            this.groupBox2.Controls.Add(this.UR1A);
            this.groupBox2.Controls.Add(this.UR19);
            this.groupBox2.Controls.Add(this.UR10);
            this.groupBox2.Location = new Point(0x196, 14);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new Size(0x12e, 0xed);
            this.groupBox2.TabIndex = 6;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Used 1";
            this.UR18.AutoSize = true;
            this.UR18.Location = new Point(6, 0xce);
            this.UR18.Name = "UR18";
            this.UR18.Size = new Size(0x27, 0x11);
            this.UR18.TabIndex = 0;
            this.UR18.TabStop = true;
            this.UR18.Text = "R8";
            this.UR18.UseVisualStyleBackColor = true;
            this.UR17.AutoSize = true;
            this.UR17.Location = new Point(6, 0xb6);
            this.UR17.Name = "UR17";
            this.UR17.Size = new Size(0x27, 0x11);
            this.UR17.TabIndex = 0;
            this.UR17.TabStop = true;
            this.UR17.Text = "R7";
            this.UR17.UseVisualStyleBackColor = true;
            this.UR16.AutoSize = true;
            this.UR16.Location = new Point(6, 160);
            this.UR16.Name = "UR16";
            this.UR16.Size = new Size(0x27, 0x11);
            this.UR16.TabIndex = 0;
            this.UR16.TabStop = true;
            this.UR16.Text = "R6";
            this.UR16.UseVisualStyleBackColor = true;
            this.UR15.AutoSize = true;
            this.UR15.Location = new Point(6, 0x88);
            this.UR15.Name = "UR15";
            this.UR15.Size = new Size(0x27, 0x11);
            this.UR15.TabIndex = 0;
            this.UR15.TabStop = true;
            this.UR15.Text = "R5";
            this.UR15.UseVisualStyleBackColor = true;
            this.UR14.AutoSize = true;
            this.UR14.Location = new Point(6, 0x71);
            this.UR14.Name = "UR14";
            this.UR14.Size = new Size(0x27, 0x11);
            this.UR14.TabIndex = 0;
            this.UR14.TabStop = true;
            this.UR14.Text = "R4";
            this.UR14.UseVisualStyleBackColor = true;
            this.UR13.AutoSize = true;
            this.UR13.Location = new Point(6, 0x5b);
            this.UR13.Name = "UR13";
            this.UR13.Size = new Size(0x76, 0x11);
            this.UR13.TabIndex = 0;
            this.UR13.TabStop = true;
            this.UR13.Text = "R3 (nicht benutzen)";
            this.UR13.UseVisualStyleBackColor = true;
            this.UR12.AutoSize = true;
            this.UR12.Location = new Point(6, 0x43);
            this.UR12.Name = "UR12";
            this.UR12.Size = new Size(0x76, 0x11);
            this.UR12.TabIndex = 0;
            this.UR12.TabStop = true;
            this.UR12.Text = "R2 (nicht benutzen)";
            this.UR12.UseVisualStyleBackColor = true;
            this.UR11.AutoSize = true;
            this.UR11.Location = new Point(6, 0x2a);
            this.UR11.Name = "UR11";
            this.UR11.Size = new Size(0x76, 0x11);
            this.UR11.TabIndex = 0;
            this.UR11.TabStop = true;
            this.UR11.Text = "R1 (nicht benutzen)";
            this.UR11.UseVisualStyleBackColor = true;
            this.UR1E.AutoSize = true;
            this.UR1E.Location = new Point(0x99, 0x89);
            this.UR1E.Name = "UR1E";
            this.UR1E.Size = new Size(0x93, 0x11);
            this.UR1E.TabIndex = 0;
            this.UR1E.TabStop = true;
            this.UR1E.Text = "LR (R14) (nicht benutzen)";
            this.UR1E.UseVisualStyleBackColor = true;
            this.UR1D.AutoSize = true;
            this.UR1D.Location = new Point(0x99, 0x71);
            this.UR1D.Name = "UR1D";
            this.UR1D.Size = new Size(0x93, 0x11);
            this.UR1D.TabIndex = 0;
            this.UR1D.TabStop = true;
            this.UR1D.Text = "SP (R13) (nicht benutzen)";
            this.UR1D.UseVisualStyleBackColor = true;
            this.UR1C.AutoSize = true;
            this.UR1C.Location = new Point(0x99, 0x5b);
            this.UR1C.Name = "UR1C";
            this.UR1C.Size = new Size(0x7c, 0x11);
            this.UR1C.TabIndex = 0;
            this.UR1C.TabStop = true;
            this.UR1C.Text = "R12 (nicht benutzen)";
            this.UR1C.UseVisualStyleBackColor = true;
            this.UR1B.AutoSize = true;
            this.UR1B.Location = new Point(0x99, 0x41);
            this.UR1B.Name = "UR1B";
            this.UR1B.Size = new Size(0x2d, 0x11);
            this.UR1B.TabIndex = 0;
            this.UR1B.TabStop = true;
            this.UR1B.Text = "R11";
            this.UR1B.UseVisualStyleBackColor = true;
            this.UR1A.AutoSize = true;
            this.UR1A.Checked = true;
            this.UR1A.Location = new Point(0x99, 0x2a);
            this.UR1A.Name = "UR1A";
            this.UR1A.Size = new Size(0x2d, 0x11);
            this.UR1A.TabIndex = 0;
            this.UR1A.TabStop = true;
            this.UR1A.Text = "R10";
            this.UR1A.UseVisualStyleBackColor = true;
            this.UR19.AutoSize = true;
            this.UR19.Location = new Point(0x99, 0x13);
            this.UR19.Name = "UR19";
            this.UR19.Size = new Size(0x27, 0x11);
            this.UR19.TabIndex = 0;
            this.UR19.TabStop = true;
            this.UR19.Text = "R9";
            this.UR19.UseVisualStyleBackColor = true;
            this.UR10.AutoSize = true;
            this.UR10.Location = new Point(6, 0x13);
            this.UR10.Name = "UR10";
            this.UR10.Size = new Size(0x76, 0x11);
            this.UR10.TabIndex = 0;
            this.UR10.TabStop = true;
            this.UR10.Text = "R0 (nicht benutzen)";
            this.UR10.UseVisualStyleBackColor = true;
            this.groupBox3.Controls.Add(this.UR28);
            this.groupBox3.Controls.Add(this.UR27);
            this.groupBox3.Controls.Add(this.UR26);
            this.groupBox3.Controls.Add(this.UR25);
            this.groupBox3.Controls.Add(this.UR24);
            this.groupBox3.Controls.Add(this.UR23);
            this.groupBox3.Controls.Add(this.UR22);
            this.groupBox3.Controls.Add(this.UR21);
            this.groupBox3.Controls.Add(this.UR2E);
            this.groupBox3.Controls.Add(this.UR2D);
            this.groupBox3.Controls.Add(this.UR2C);
            this.groupBox3.Controls.Add(this.UR2B);
            this.groupBox3.Controls.Add(this.UR2A);
            this.groupBox3.Controls.Add(this.UR29);
            this.groupBox3.Controls.Add(this.UR20);
            this.groupBox3.Location = new Point(0x2ca, 14);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new Size(0x12e, 0xed);
            this.groupBox3.TabIndex = 7;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Used 2";
            this.UR28.AutoSize = true;
            this.UR28.Location = new Point(6, 0xce);
            this.UR28.Name = "UR28";
            this.UR28.Size = new Size(0x27, 0x11);
            this.UR28.TabIndex = 0;
            this.UR28.TabStop = true;
            this.UR28.Text = "R8";
            this.UR28.UseVisualStyleBackColor = true;
            this.UR27.AutoSize = true;
            this.UR27.Location = new Point(6, 0xb6);
            this.UR27.Name = "UR27";
            this.UR27.Size = new Size(0x27, 0x11);
            this.UR27.TabIndex = 0;
            this.UR27.TabStop = true;
            this.UR27.Text = "R7";
            this.UR27.UseVisualStyleBackColor = true;
            this.UR26.AutoSize = true;
            this.UR26.Location = new Point(6, 160);
            this.UR26.Name = "UR26";
            this.UR26.Size = new Size(0x27, 0x11);
            this.UR26.TabIndex = 0;
            this.UR26.TabStop = true;
            this.UR26.Text = "R6";
            this.UR26.UseVisualStyleBackColor = true;
            this.UR25.AutoSize = true;
            this.UR25.Location = new Point(6, 0x88);
            this.UR25.Name = "UR25";
            this.UR25.Size = new Size(0x27, 0x11);
            this.UR25.TabIndex = 0;
            this.UR25.TabStop = true;
            this.UR25.Text = "R5";
            this.UR25.UseVisualStyleBackColor = true;
            this.UR24.AutoSize = true;
            this.UR24.Location = new Point(6, 0x71);
            this.UR24.Name = "UR24";
            this.UR24.Size = new Size(0x27, 0x11);
            this.UR24.TabIndex = 0;
            this.UR24.TabStop = true;
            this.UR24.Text = "R4";
            this.UR24.UseVisualStyleBackColor = true;
            this.UR23.AutoSize = true;
            this.UR23.Location = new Point(6, 0x5b);
            this.UR23.Name = "UR23";
            this.UR23.Size = new Size(0x76, 0x11);
            this.UR23.TabIndex = 0;
            this.UR23.TabStop = true;
            this.UR23.Text = "R3 (nicht benutzen)";
            this.UR23.UseVisualStyleBackColor = true;
            this.UR22.AutoSize = true;
            this.UR22.Location = new Point(6, 0x43);
            this.UR22.Name = "UR22";
            this.UR22.Size = new Size(0x76, 0x11);
            this.UR22.TabIndex = 0;
            this.UR22.TabStop = true;
            this.UR22.Text = "R2 (nicht benutzen)";
            this.UR22.UseVisualStyleBackColor = true;
            this.UR21.AutoSize = true;
            this.UR21.Location = new Point(6, 0x2a);
            this.UR21.Name = "UR21";
            this.UR21.Size = new Size(0x76, 0x11);
            this.UR21.TabIndex = 0;
            this.UR21.TabStop = true;
            this.UR21.Text = "R1 (nicht benutzen)";
            this.UR21.UseVisualStyleBackColor = true;
            this.UR2E.AutoSize = true;
            this.UR2E.Location = new Point(0x99, 0x89);
            this.UR2E.Name = "UR2E";
            this.UR2E.Size = new Size(0x93, 0x11);
            this.UR2E.TabIndex = 0;
            this.UR2E.TabStop = true;
            this.UR2E.Text = "LR (R14) (nicht benutzen)";
            this.UR2E.UseVisualStyleBackColor = true;
            this.UR2D.AutoSize = true;
            this.UR2D.Location = new Point(0x99, 0x71);
            this.UR2D.Name = "UR2D";
            this.UR2D.Size = new Size(0x93, 0x11);
            this.UR2D.TabIndex = 0;
            this.UR2D.TabStop = true;
            this.UR2D.Text = "SP (R13) (nicht benutzen)";
            this.UR2D.UseVisualStyleBackColor = true;
            this.UR2C.AutoSize = true;
            this.UR2C.Location = new Point(0x99, 0x5b);
            this.UR2C.Name = "UR2C";
            this.UR2C.Size = new Size(0x7c, 0x11);
            this.UR2C.TabIndex = 0;
            this.UR2C.TabStop = true;
            this.UR2C.Text = "R12 (nicht benutzen)";
            this.UR2C.UseVisualStyleBackColor = true;
            this.UR2B.AutoSize = true;
            this.UR2B.Checked = true;
            this.UR2B.Location = new Point(0x99, 0x41);
            this.UR2B.Name = "UR2B";
            this.UR2B.Size = new Size(0x2d, 0x11);
            this.UR2B.TabIndex = 0;
            this.UR2B.TabStop = true;
            this.UR2B.Text = "R11";
            this.UR2B.UseVisualStyleBackColor = true;
            this.UR2A.AutoSize = true;
            this.UR2A.Location = new Point(0x99, 0x2a);
            this.UR2A.Name = "UR2A";
            this.UR2A.Size = new Size(0x2d, 0x11);
            this.UR2A.TabIndex = 0;
            this.UR2A.TabStop = true;
            this.UR2A.Text = "R10";
            this.UR2A.UseVisualStyleBackColor = true;
            this.UR29.AutoSize = true;
            this.UR29.Location = new Point(0x99, 0x13);
            this.UR29.Name = "UR29";
            this.UR29.Size = new Size(0x27, 0x11);
            this.UR29.TabIndex = 0;
            this.UR29.TabStop = true;
            this.UR29.Text = "R9";
            this.UR29.UseVisualStyleBackColor = true;
            this.UR20.AutoSize = true;
            this.UR20.Location = new Point(6, 0x13);
            this.UR20.Name = "UR20";
            this.UR20.Size = new Size(0x76, 0x11);
            this.UR20.TabIndex = 0;
            this.UR20.TabStop = true;
            this.UR20.Text = "R0 (nicht benutzen)";
            this.UR20.UseVisualStyleBackColor = true;
            this.onlyPUemulation.AutoSize = true;
            this.onlyPUemulation.Checked = true;
            this.onlyPUemulation.CheckState = CheckState.Checked;
            this.onlyPUemulation.Location = new Point(0x404, 150);
            this.onlyPUemulation.Name = "onlyPUemulation";
            this.onlyPUemulation.Size = new Size(0x3e, 0x11);
            this.onlyPUemulation.TabIndex = 8;
            this.onlyPUemulation.Text = "von PU";
            this.onlyPUemulation.UseVisualStyleBackColor = true;
            this.checkBox2.AutoSize = true;
            this.checkBox2.Location = new Point(0x404, 0xad);
            this.checkBox2.Name = "checkBox2";
            this.checkBox2.Size = new Size(0x58, 0x11);
            this.checkBox2.TabIndex = 8;
            this.checkBox2.Text = "von Kassette";
            this.checkBox2.UseVisualStyleBackColor = true;
            this.Droc.AutoSize = true;
            this.Droc.Location = new Point(12, 12);
            this.Droc.Name = "Droc";
            this.Droc.Size = new Size(0x6b, 0x11);
            this.Droc.TabIndex = 0;
            this.Droc.Text = "Direct read offset";
            this.Droc.UseVisualStyleBackColor = true;
            this.Droc.CheckedChanged += new EventHandler(this.Droc_CheckedChanged);
            base.AutoScaleDimensions = new SizeF(6f, 13f);
            base.AutoScaleMode = AutoScaleMode.Font;
            base.ClientSize = new Size(0x486, 0x106);
            base.Controls.Add(this.checkBox2);
            base.Controls.Add(this.onlyPUemulation);
            base.Controls.Add(this.groupBox3);
            base.Controls.Add(this.groupBox2);
            base.Controls.Add(this.groupBox1);
            base.Controls.Add(this.pugba);
            base.Controls.Add(this.punds);
            base.Controls.Add(this.spchange);
            base.Controls.Add(this.noirq);
            base.Controls.Add(this.saveCPSR);
            base.Controls.Add(this.button2);
            base.Controls.Add(this.button1);
            base.Controls.Add(this.listBox1);
            base.Controls.Add(this.Droc);
            base.Name = "Form3";
            this.Text = "Form3";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            base.ResumeLayout(false);
            base.PerformLayout();
        }

        private bool need_swap(uint instr)
        {
            bool flag = false;
            if ((((instr & 0xe000000) == 0) && ((instr & 0x100000) != 0x100000)) && ((instr & 0x60) == 0x20))
            {
                flag = true;
            }
            if (((instr & 0xc000000) == 0x4000000) && ((instr & 0x100000) != 0x100000))
            {
                flag = true;
            }
            if (!flag)
            {
                return false;
            }
            if (((instr & 0xf000) == 0x1000) || (((instr & 0xf000) >> 12) != 0))
            {
                return false;
            }
            if (((instr & 0xf0000) >> 0x10) == 1)
            {
                MessageBox.Show("critical error str r0 and r1");
            }
            return true;
        }

        private void spchange_CheckedChanged(object sender, EventArgs e)
        {
            if (this.spchange.Checked)
            {
                this.Z13.Enabled = true;
                this.Z13.Checked = false;
            }
            else
            {
                this.Z13.Enabled = false;
                this.Z13.Checked = true;
            }
        }

        public void storu32(uint tostr, ref byte[] fild, uint offset)
        {
            fild[offset] = (byte) tostr;
            fild[(int) ((IntPtr) (offset + 1))] = (byte) (tostr >> 8);
            fild[(int) ((IntPtr) (offset + 2))] = (byte) (tostr >> 0x10);
            fild[(int) ((IntPtr) (offset + 3))] = (byte) (tostr >> 0x18);
        }
    }
}

