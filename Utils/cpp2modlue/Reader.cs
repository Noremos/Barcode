using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Parcsh
{

    public abstract class SourceCodeReader
    {
        abstract public void analysFile(string filepath);
    };

    public class CppReader : SourceCodeReader
    {
        protected CppParser parser = new CppParser();

        public List<classInfo> classes = new List<classInfo>();

        static bool check(string a)
        {
            return a.Length != 0;
        }

        static bool check(baseFunc a)
        {
            return a != null;
        }

        static bool check(argInfo a)
        {
            return a != null;
        }
        static bool check(classInfo a)
        {
            return a != null;
        }

        protected string getArg(argInfo info)
        {
            string ret = parser.getTypeFromKey(info.type) + " " + info.name;
            if (info.defaulValue.Length != 0)
                ret += " = " + info.defaulValue;
            return ret;
        }

        const int maxSize = 100;
        string getDict(string type)
        {
            return type + "* dictFor" + type + "[" + maxSize.ToString() + "];int last" + type + "=0;";
        }

        public override void analysFile(string filepath) 
        {
            StreamReader sr = new StreamReader(filepath);
            string line;
            bool avlZone = false;
            bool startClass = false;

            classInfo classInfo = null;

            int skobs = 0;
            int funSkob = -1, skobsCountWhereClassStarts = -1;
            bool multiComm = false;


            void checkSkobsFnd(ps neededSkob, ref int initOfFnd)
            {
                ps curTe = parser.lineInfo.getSign(0);
                int inde = 1;
                int sd = skobs;
                while (curTe != ps.end)
                {
                    if (curTe == neededSkob)
                        initOfFnd = skobs;
                    if (curTe == ps.fig_skob_op)
                        skobs++;
                    else if (curTe == ps.fig_skob_cl)
                        skobs--;
                    curTe = parser.lineInfo.getSign(inde++);
                }

                if (skobs == sd)
                    return;
                if (skobsCountWhereClassStarts != -1)
                {
                    if (skobs < skobsCountWhereClassStarts)
                    {
                        skobsCountWhereClassStarts = -1;
                        classInfo = null;
                    }
                    // !!!!!!!!!!!!!!!!!!
                }
                if (funSkob != -1)
                {
                    if (skobs <= funSkob)
                        funSkob = -1;
                }
            }

            void checkSkobs()
            {
                ps curTe = parser.lineInfo.getSign(0);
                int inde = 1;
                int startCount = skobs;
                while (curTe != ps.end)
                {
                    if (curTe == ps.fig_skob_op)
                        skobs++;
                    else if (curTe == ps.fig_skob_cl)
                        skobs--;
                    curTe = parser.lineInfo.getSign(inde++);
                }
                if (startCount <= skobs)
                    return;

                if (skobsCountWhereClassStarts != -1)
                {
                    if (skobs < skobsCountWhereClassStarts)
                    {
                        skobsCountWhereClassStarts = -1;
                        classInfo = null;
                    }
                    // !!!!!!!!!!!!!!!!!!
                }
                if (funSkob != -1)
                {
                    if (skobs < funSkob)
                        funSkob = -1;
                }
            }


            int zone = 0;

            const int classZone = 5;
            const int funcZone = 6;

            string templ = "";
            while ((line = sr.ReadLine()) != null)
            {
                line = line.Trim();
                if (multiComm)
                {
                    int indcom = line.IndexOf("*/");
                    if (indcom == -1)
                        continue;

                    line = line[(indcom + 2)..];
                }
                while (line.EndsWith(','))
                {
                    line += sr.ReadLine().Trim();
                }

                if (line.Contains("clone("))
                    Console.WriteLine();
                if (!parser.getSignature(line, out multiComm))
                    continue;

                // TEMPLATE
                //string temp = parser.getTemplate();
                //if (check(temp))
                //{
                //    //Debug.Assert(templ.Length == 0);
                //    templ = temp;
                //    continue;
                //}

                string clas = parser.getClassProto();
                if (check(clas))
                {
                    templ = "";
                    parser.getTypeId(clas);
                    checkSkobs();
                    continue;
                }

                clas = parser.getClass();
                if (check(clas))
                {
                    classInfo = new classInfo
                    {
                        name = clas
                    };
                    parser.getTypeId(clas);

                    classes.Add(classInfo);

                    avlZone = false;
                    //skobs = 0;
                    if (check(templ))
                    {
                        classInfo.tempType = templ;
                        templ = "";
                    }
                    //skobsCountWhereClassStarts = 0;
                    skobsCountWhereClassStarts = skobs + 1;
                    checkSkobs();
                    //checkSkobsFnd(ps.pclass, ref skobsCountWhereClassStarts);
                    continue;
                }

                if (parser.isUsing())
                {
                    templ = "";
                    continue;
                }
                checkZone(line, ref avlZone);


                if (check(classInfo))
                {
                    if (avlZone && (funSkob == -1) && skobs == skobsCountWhereClassStarts)
                    {
                        var membr = parser.getMember();
                        if (check(membr))
                        {
                            int mem = 0;
                            classInfo.members.Add(membr);

                            checkSkobs();
                            continue;
                        }

                        var conct = parser.getConstr(classInfo.name);
                        if (check(conct))
                        {
                            if (!parser.lineInfo.contains(ps.je))
                            {
                                // Есть тело
                                funSkob = skobs + 1;
                            }
                            classInfo.consrts.Add(conct);
                            checkSkobs();
                            continue;
                        }

                        var destr = parser.getDestruct(classInfo.name);
                        if (check(destr))
                        {
                            if (!parser.lineInfo.contains(ps.je))
                            {
                                // Есть тело
                                funSkob = skobs + 1;
                            }
                            Debug.Assert(classInfo.destructor == null);
                            classInfo.destructor = destr;
                            checkSkobs();
                            continue;
                        }


                        //"bc::BarRoot* getRootNode()"
                        var func = parser.getFunc();
                        if (check(func))
                        {
                            if (check(templ))
                            {
                                func.tempType = templ;
                                templ = "";
                            }

                            classInfo.funcs.Add(func);
                            if (!parser.lineInfo.contains(ps.je))
                            {
                                // Есть тело
                                funSkob = skobs + 1;
                            }
                            checkSkobs();
                            continue;
                        }
                    }
                }

                checkSkobs();
            }
        }

        static readonly string[] zoneStatments = { "public:", "private:", "protected:" };

        bool checkZone(string line, ref bool curZone)
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

    }
    public class CppPrinter : CppReader
    {

        static readonly string[] classStatments = { "class EXPORT", "struct EXPORT" };


        int findNextSymbol(string line, char symb, int start = 0)
        {
            while (start < line.Length && line[start++] != symb) ;
            return start;
        }

        string getClassName(string line, ref bool startClass)
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

        public void printCppClasses()
        {
            foreach (var cla in classes)
            {
                if (cla.tempType?.Length != 0)
                {
                    Console.WriteLine("template<class {0}>", cla.tempType);
                }
                Console.WriteLine("class EXPORT {0}", cla.name);
                Console.WriteLine("{");

                Console.WriteLine("\t// Constructors:");

                foreach (var fun in cla.consrts)
                {
                    if (fun.tempType.Length != 0)
                        Console.WriteLine("\ttemplate<class {0}>", fun.tempType);

                    string ret = "\t";
                    ret += " " + cla.name + "(";

                    foreach (var arg in fun.args)
                        ret += getArg(arg) + ", ";

                    if (fun.args.Count != 0)
                        ret = ret[..(ret.Length - 2)];
                    ret += ");";

                    Console.WriteLine(ret);
                    Console.WriteLine("");
                }

                Console.WriteLine("\t// Desructor:");

                if (cla.destructor != null)
                {
                    Console.WriteLine("\t{0}~{1}();", (cla.destructor.isVirual ? "virtual " : ""), cla.name);
                }
                Console.WriteLine("");


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

                    ret += parser.getTypeFromKey(fun.retType);

                    ret += " " + fun.name + "(";
                    foreach (var arg in fun.args)
                    {
                        ret += getArg(arg) + ", ";
                    }
                    if (fun.args.Count != 0)
                        ret = ret[..(ret.Length - 2)];
                    ret += ")";

                    if (fun.isConst)
                    {
                        ret += " const";
                    }

                    if (fun.isOverride)
                    {
                        ret += " override";
                    }

                    ret += ";";
                    Console.WriteLine(ret);
                }
                Console.WriteLine("}");
                Console.WriteLine("");
                Console.WriteLine("");

            }
        }
    }
}