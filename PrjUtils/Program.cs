using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace BarWrapperUtility
{
    class Program
    {
        static string pathToSource;

        static void computePath()
        {
            pathToSource = Environment.CurrentDirectory;
            string folder = "PrjBarlib";
            while (!Directory.Exists(Path.Combine(pathToSource, folder)))
            {
                pathToSource = Path.GetFullPath(Path.Combine(pathToSource, @"..\"));
                if (Path.GetPathRoot(pathToSource) == pathToSource)
                {
                    return;
                }
            }
            pathToSource = Path.Combine(pathToSource, folder + "/include");
        }


        string[] fileblacklist = { "Component.h", "Hole.h", "include_cv.h", "include_py.h" };
        string[] classesBlackList = { "BarMat", "BarNdarray" };


        static readonly string[] classStatments = { "class EXPORT", "struct EXPORT" };
        static readonly string[] zoneStatments = { "public:", "private:", "protected:" };


        static int findNextSymbol(string line, char symb, int start = 0)
        {
            while (start < line.Length && line[start++] != symb) ;
            return start;
        }
        static string checkTemplteType(string line)
        {
            string stmt = "template";
            int ind = line.IndexOf(stmt);
            if (ind != -1)
            {
                ind = line.IndexOf('<', ind);
                if (ind != -1)
                {
                    ind = line.IndexOf("class", ind + 1);
                    if (ind != -1)
                    {
                        int ind2 = line.IndexOf('>', ind);
                        if (ind2 != -1)
                        {
                            ind = ind + "class".Length;
                            return line[ind..ind2].Trim();
                        }
                    }
                }
            }
            return "";
        }
        static string getClassName(string line, ref bool startClass)
        {
            string curClass = "";
            foreach (var stmt in classStatments)
            {
                int ind = line.IndexOf(stmt);
                if (ind != -1)
                {
                    int st = ind + stmt.Length;
                    while (line[++st] == ' ') ;

                    int ed = st;
                    while (ed < line.Length && char.IsLetter(line[ed++])) ;
                    curClass = line[st..ed];

                    startClass = true;
                    while (ed < line.Length)
                    {
                        if (line[ed++] == ';')
                        {
                            startClass = false;
                            break;
                        }
                    }
                    break;
                }
            }
            return curClass.Trim();
        }
        static bool checkZone(string line, ref bool curZone)
        {
            bool goodZone = true;
            foreach (var stmt in zoneStatments)
            {
                int ind = line.IndexOf(stmt);
                if (ind != -1)
                {
                    curZone = goodZone;
                    return true;
                }
                goodZone = false;
            }
            return false;
        }

        class classInfo
        {
            public string name;
            public string tempType;

            public List<func> funcs = new List<func>();
            public List<argInfo> members = new List<argInfo>();
        }
        class argInfo
        {
            public argInfo(int _typeId, string _name, string _defValue)
            {
                type = _typeId;
                name = _name;
                defaulValue = _defValue;
            }
            //public bool isConst;
            public int type;
            public string name;
            public string defaulValue;
        }

        class func
        {
            public int retType;
            public string name;
            public string tempType;
            public bool isConst;
            public bool isVirual;
            public bool isOverride;
            public List<argInfo> args = new List<argInfo>();
        }

        class Parcer
        {
            byte ye = 1;

            byte pc_word, pc_tr_skob_wor, pc_fig_skob_op, pc_fig_skob_cl, pc_skob_word, pc_ravn,
                 pc_zero, pc_virt, pc_static, pc_const, pc_je, pc_ref, pc_export, pc_class,
                 pc_using, pc_typedef, pc_template, pc_overide, pc_end;

            Dictionary<string, byte> wordParcer;

            Parcer()
            {
                byte ye = 1;

                pc_word = ye++;//
                pc_tr_skob_wor = ye++; // <..>
                pc_fig_skob_op = ye++;//{
                pc_fig_skob_cl = ye++;//}
                pc_skob_word = ye++;//(...)
                pc_ravn = ye++;//=
                pc_zero = ye++;//0
                pc_virt = ye++;//virtual
                pc_static = ye++;//static
                pc_const = ye++;//const
                pc_je = ye++;//;
                pc_ref = ye++;// * or &
                pc_export = ye++;// EXPORT
                pc_class = ye++;// class
                pc_using = ye++;// using
                pc_typedef = ye++;// tytedef
                pc_template = ye++;// template
                pc_overide = ye++; //overide
                pc_end = ye++;// end of seq

                wordParcer = new Dictionary<string, byte>();
                wordParcer.Add("virtual", pc_virt);
                wordParcer.Add("sattic", pc_static);
                wordParcer.Add("const", pc_const);
                wordParcer.Add("EXPORT", pc_export);
                wordParcer.Add("class", pc_class);
                wordParcer.Add("using", pc_using);
                wordParcer.Add("typedef", pc_typedef);
                wordParcer.Add("template", pc_template);
                wordParcer.Add("overide", pc_overide);
            }


            bool compiteSigs(byte[] baseSig, byte[] inputSig)
            {
                if (inputSig.Length > baseSig.Length)
                    return false;
                int baseInd = 0;
                int inpInd = 0;

                Stack<(int, int)> savePoint = new Stack<(int, int)>();

                while (true)
                {
                    if (baseInd >= baseSig.Length)
                        return false;

                    if (inpInd >= inputSig.Length)
                        return false;

                    if (baseSig[baseInd] == 0)
                    {
                        savePoint.Push((baseInd + 2, inpInd));
                        ++baseInd;
                    }

                    byte baseChar = baseSig[baseInd];
                    byte inpChar = inputSig[inpInd];
                    Debug.Assert(inpChar != 0);

                    if (baseChar == inpChar)
                    {
                        if (inpChar == pc_end)
                            return true;

                        baseInd++;
                        inpInd++;
                    }
                    else
                    {
                        if (savePoint.Count > 0)
                        {
                            (baseInd, inpInd) = savePoint.Pop();
                            continue;
                        }
                    }
                }
            }

            bool isFunc(byte[] sig)
            {
                // [virual] [const] void [*]  v(...) [const] [overide] [;]
                byte[] funcFig = new byte[] { 0, pc_virt, 0, pc_const, pc_word, 0, pc_ref, pc_skob_word, 0, pc_const, pc_ravn, pc_zero, pc_je };
                return compiteSigs(funcFig, sig);
            }

            bool isAbstructFunc(byte[] sig)
            {
                //[virual] [const] void <T> [*]  v(...) [const]  = 0;
                byte[] abstFuncFig = new byte[] { 0, pc_virt, 0, pc_const, pc_word, 0, pc_template, 0, pc_ref, pc_skob_word, 0, pc_const, pc_ravn, pc_zero, pc_je };
                return compiteSigs(abstFuncFig, sig);

            }

            bool isMember(byte[] sig)
            {
                // [const] void <T> * mem = 4;
                byte[] memberFig = new byte[] { 0, pc_const, pc_word, 0, pc_ref, pc_word, 0, pc_template, pc_word, 0, pc_ravn, 0, pc_word, pc_je };
                return compiteSigs(memberFig, sig);

            }

            bool isClass(byte[] sig)
            {
                // class Exopr cl1
                byte[] classSig = new byte[] { pc_class, pc_export, pc_word };
                return compiteSigs(classSig, sig);

            }

            bool isClassProto(byte[] sig)
            {
                // class Exopr cl1
                byte[] classProtoSig = new byte[] { pc_class, 0, pc_export, pc_word, pc_je };
                return compiteSigs(classProtoSig, sig);
            }

            bool isTemplate(byte[] sig)
            {
                // template <class t>
                byte[] tempateSig = new byte[] { pc_template, pc_word };
                return compiteSigs(tempateSig, sig);
            }

            bool isUsing(byte[] sig)
            {
                // using a = b<T>*;
                byte[] usingSig = new byte[] { pc_using, pc_word, pc_ravn, 0, pc_template, 0, pc_ref, pc_je };
                return compiteSigs(usingSig, sig);
            }

            bool isTypedef(byte[] sig)
            {
                // typedef bc::DatagridProvider < T >* bcBarImg;
                byte[] typedefSig = new byte[] { pc_typedef, pc_word, 0, pc_template, 0, pc_ref, pc_word, pc_je };
                return compiteSigs(typedefSig, sig);
            }

            (byte[], Range[]) getSignature(string line)
            {
                byte[] pcpars = new byte[30];
                Range[] pozs = new Range[30];
                int curl = 0;
                bool word = false;
                int workSt = -1;
                for (int i = 0; i < line.Length; i++)
                {
                    void closeWord()
                    {
                        if (word)
                        {
                            string wordstr = line[workSt..i];
                            byte code;
                            if (wordParcer.TryGetValue(wordstr, out code))
                                pcpars[curl++] = pc_virt;
                            else
                                pcpars[curl++] = pc_word;

                            word = false;
                            pozs[curl] = new Range(workSt, i);

                        }
                        else
                            workSt = i;
                    }

                    char c = line[i];
                    if (c == '\t' || c == ' ' || c == '\n' || c == '\r' || c == '\n')
                    {
                        closeWord();
                    }

                    if (c == '(')
                    {
                        closeWord();
                        while (i < line.Length && line[i] != ')')
                            continue;

                        Debug.Assert(i != line.Length);
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_skob_word;
                    }

                    if (c == '=')
                    {
                        closeWord();
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_ravn;
                    }

                    if (c == ';')
                    {
                        closeWord();
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_je;
                    }


                    if (c == '<')
                    {
                        closeWord();
                        int coitr = 1;
                        while (i < line.Length && coitr != 0)
                        {
                            if (line[i] != '>')
                                coitr--;
                            else if (line[i] != '<')
                            {
                                coitr++;
                            }
                        }
                        Debug.Assert(i != line.Length);
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_skob_word;
                        continue;
                    }

                    if (c == '{')
                    {
                        closeWord();
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_fig_skob_op;
                    }

                    if (c == '}')
                    {
                        closeWord();
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_fig_skob_cl;
                    }

                    if (c == '*' || c == '&')
                    {
                        closeWord();
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_ref;
                    }

                    if (c == '0')
                    {
                        closeWord();
                        pozs[curl] = new Range(i, i);
                        pcpars[curl++] = pc_zero;
                    }
                }

                pozs[curl] = new Range(line.Length, line.Length);
                pcpars[curl++] = pc_end;

                return (pcpars, pozs);
            }
        }

        static string getNextWord(ref string line, int start = 0)
        {
            if (line.Length == 0)
                return line;

            while (start < line.Length && !char.IsLetter(line[start++])) ;

            int end = start--;
            while (end < line.Length && char.IsLetter(line[end++])) ;
            end--;

            string word = line[start..end];
            line = line[(end + 1)..];
            return word;
        }

        static Dictionary<string, int> types = new Dictionary<string, int>();

        static int getTypeId(string type, string classTemplate = "", string funcTemplate = "")
        {
            type = type.Trim();

            if (type == classTemplate)
                return -1;

            if (type == funcTemplate)
                return -200;

            int id;
            if (!types.TryGetValue(type, out id))
            {
                id = types.Count;
                types.Add(type, id);
            }
            return id;
        }

        static int getNenZeroIndex(string line)
        {
            int start = 0;
            while (start < line.Length && !char.IsLetter(line[start++])) ;
            if (start == line.Length)
                return -1;
            return start - 1;
        }


        static string getNextSpecialValue(ref string ln)
        {
            ln = ln.Trim();
            if (ln.Length == 0)
                return "";
            int st = 0;
            while (st < ln.Length && (ln[st] != ' ' && ln[st] != '*' && ln[st] != '&')) ++st;
            if (st == ln.Length)
            {
                string word = ln;
                ln = "";
                return word.Trim();
            }
            else
            {
                string word = ln[..st];
                ln = ln[st..];
                return word.Trim();
            }
        }

        static bool parceMember(string _tyarg, string classTemplate, string funcTemplate, out argInfo info)
        {
            info = null;

            string tyarg = _tyarg.Trim();
            if (tyarg.Length == 0 || tyarg.IndexOf('(') != -1)
                return false;

            if (tyarg == "const barbase *bc")
                Console.WriteLine();

            string argtype;
            string name;
            string defValue = "";


            string word = getNextSpecialValue(ref tyarg);
            if (word.Length == 0)
                return false;

            if (word == "const")
            {
                word = getNextSpecialValue(ref tyarg);
                Debug.Assert(word.Length != 0);
                argtype = "const " + word;
            }
            else
                argtype = word;

            tyarg = tyarg.Trim();
            if (tyarg.Length == 0)
                return false;

            if (tyarg[0] == '*' || tyarg[0] == '&')
            {
                argtype += tyarg[0];
                tyarg = tyarg[1..];
            }

            word = getNextSpecialValue(ref tyarg);
            if (word.Length == 0)
                return false;

            if (word == "const")
            {
                argtype += " const";

                tyarg = tyarg.Trim();

                if (tyarg[0] == '*' || tyarg[0] == '&')
                {
                    argtype += tyarg[0];
                    tyarg = tyarg[1..];
                }
                word = getNextSpecialValue(ref tyarg);
            }

            name = word;
            if (name.Contains('('))
                return false;

            word = getNextSpecialValue(ref tyarg);
            if (word == "=")
            {
                defValue = getNextSpecialValue(ref tyarg);
                Debug.Assert(defValue.Length != 0);
            }
            // const T* val = null;

            int typeId = getTypeId(argtype, classTemplate, funcTemplate);
            info = new argInfo(typeId, name, defValue);
            return true;
        }

        static bool checkFunction(string line, out func onclass, string funcTemplate, string classTemplate)
        {
            string type = getNextWord(ref line);

            bool virt = false;
            if (type == "virtual")
            {
                virt = true;
                type = getNextWord(ref line);
            }
            bool sta = false;
            if (type == "static")
            {
                sta = true;
                type = getNextWord(ref line);
            }

            if (type == "virtual")
            {
                virt = true;
                type = getNextWord(ref line);
            }
            onclass = null;
            if (virt == false && sta == true)
                return false;

            onclass = new func
            {
                retType = getTypeId(type, funcTemplate, classTemplate),
                tempType = funcTemplate
            };

            int start = getNenZeroIndex(line);

            if (start == -1)
                return false;

            int funcNameEnd = line.IndexOf('(');
            if (funcNameEnd == -1 || funcNameEnd < start) return false;

            onclass.name = line[start..funcNameEnd].Trim();
            line = line[(funcNameEnd + 1)..];

            int funcAgrEnd = line.IndexOf(')');
            if (funcAgrEnd == -1) return false;

            string postfun = line[funcAgrEnd..];
            int indend = 0;
            byte ciu = 0;
            while (indend < postfun.Length)
            {
                if (postfun[indend] == '=' || postfun[indend] == '0')
                    ++ciu;
                ++indend;
            }

            // func name() = 0;
            if (ciu == 2)
                return false;

            line = line[..funcAgrEnd].Trim();


            string sourceLine = line;
            string[] typeargs = line.Split(',');
            foreach (var _tyarg in typeargs)
            {
                string tyarg = _tyarg.Trim();
                if (tyarg.Length == 0)
                    continue;

                argInfo inf;
                if (parceMember(tyarg, classTemplate, funcTemplate, out inf))
                {
                    onclass.args.Add(inf);
                }
            }

            return true;
        }
        static List<classInfo> classes = new List<classInfo>();

        static void analysFile(string filepath)
        {
            StreamReader sr = new StreamReader(filepath);
            string line;
            bool avlZone = false;
            bool startClass = false;

            classInfo classInfo = null;

            int skobs = 0;
            int funSkob = -1, classSkob = -1;
            while ((line = sr.ReadLine()) != null)
            {
                string tyemplName = checkTemplteType(line);
                if (tyemplName.Length != 0)
                {
                    line = sr.ReadLine();
                }
                if (line.Contains("using ") || line.Contains("typedef"))
                    continue;

                string curClass = getClassName(line, ref startClass);
                if (curClass.Length != 0)
                {
                    getTypeId(curClass);
                    if (line.Contains(';'))
                    {
                        continue;
                    }

                    classInfo = new classInfo
                    {
                        name = curClass
                    };

                    classes.Add(classInfo);

                    avlZone = false;
                    //skobs = 0;
                    classSkob = skobs;
                    if (tyemplName.Length != 0)
                    {
                        classInfo.tempType = tyemplName;
                        tyemplName = "";
                    }

                    if (line.Contains('{'))
                        skobs++;
                    continue;
                }



                if (classInfo != null)
                {
                    if (checkZone(line, ref avlZone))
                        continue;

                    if (skobs == classSkob + 1 && avlZone)
                    {

                        //byte[] sign = getSignature(line);
                        argInfo info;
                        if (parceMember(line, classInfo.tempType, "", out info))
                        {
                            //func fSet = new func();
                            //fSet.name = "set" + char.ToUpper(info.name[0]) + info.name[1..];
                            classInfo.members.Add(info);
                        }

                        func functi;
                        if (checkFunction(line, out functi, tyemplName, classInfo.tempType))
                        {
                            tyemplName = "";
                            Debug.Assert(classInfo != null);
                            classInfo.funcs.Add(functi);

                            if (!line.Trim().EndsWith(';'))
                            {
                                funSkob = skobs;
                            }
                        }
                    }
                }

                int ind = 0;
                while (ind < line.Length)
                {
                    if (line[ind] == '{')
                    {
                        ++skobs;
                    }
                    else if (line[ind] == '}')
                    {
                        --skobs;
                        Debug.Assert(skobs >= 0);
                    }
                    ++ind;
                }
            }
        }
        static void Main(string[] args)
        {
            computePath();

            foreach (var filename in Directory.GetFiles(pathToSource))
            {
                string filepath = Path.Combine(pathToSource, filename);
                analysFile(filepath);
            }

            foreach (var cla in classes)
            {
                if (cla.tempType?.Length != 0)
                {
                    Console.WriteLine("template<class {0}>", cla.name);
                }
                Console.WriteLine("class EXPORT {0}", cla.name);
                Console.WriteLine("{");


                string getArg(argInfo info)
                {
                    string ret = info.type + " " + info.name;
                    if (info.defaulValue.Length != 0)
                        ret += " = " + info.defaulValue;
                    return ret;
                }

                Console.WriteLine("\t// Members:");

                foreach (var memb in cla.members)
                {
                    string ret = "\t" + getArg(memb) + ";";
                    Console.WriteLine(ret);
                    Console.WriteLine("");
                }

                Console.WriteLine("\t// Functions:");

                foreach (var fun in cla.funcs)
                {
                    if (fun.tempType.Length != 0)
                    {
                        Console.WriteLine("\ttemplate<class {0}>", fun.tempType);
                    }

                    string ret = "\t";
                    if (fun.isVirual)
                    {
                        ret += "virtual ";
                    }
                    ret += fun.name + "(";
                    foreach (var arg in fun.args)
                    {
                        ret += getArg(arg) + ", ";
                    }
                    ret = ret[..(ret.Length - 2)];
                    ret += ")";

                    if (fun.isConst)
                    {
                        ret += " const";
                    }

                    if (fun.isOverride)
                    {
                        ret += " overide";
                    }

                    ret += ";";
                    Console.WriteLine(ret);
                }
                Console.WriteLine("}");
                Console.WriteLine("");
                Console.WriteLine("");

            }
            Console.WriteLine("Hello World!");

        }
    }
}
