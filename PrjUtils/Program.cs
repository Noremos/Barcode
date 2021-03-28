using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

public class classInfo
{
    public string name;
    public string tempType;

    public List<func> funcs = new List<func>();
    public List<argInfo> members = new List<argInfo>();
}
public class argInfo
{
    public argInfo(int _typeId = 0, string _name = "", string _defValue = "")
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

public class func
{
    public int retType;
    public string name;
    public string tempType = "";
    public bool isConst;
    public bool isVirual;
    public bool isOverride;
    public List<argInfo> args = new List<argInfo>();
}


public class parcInfo
{
    public string line;
    public ps[] sign = null;
    public Range[] pozs = null;

    public Range? getRangeWhere(ps bc_ind, int start = 0)
    {
        for (; start < sign.Length; ++start)
        {
            if (sign[start] == bc_ind)
                return pozs[start];
        }
        return null;
    }

    public ps getSign(int ind)
    {
        return sign[ind];
    }
    public int getPozSt(int ind)
    {
        return pozs[ind].Start.Value;
    }
    public int getPozEd(int ind)
    {
        return pozs[ind].End.Value;
    }

    public string getSub(int ind)
    {
        return line[getPozSt(ind)..getPozEd(ind)];
    }

    public bool isSign(int poz, ps pc_ind)
    {
        return sign[poz] == pc_ind;
    }

    public string getFullType(ref int startnd)
    {
        // [const] void [< T >] [*]  name
        int couMun = 0;
        while (true)
        {
            if (isSign(startnd, ps.word))
            {
                couMun++;
                if (couMun == 2)
                    break;
            }
            ++startnd;
        }

        return line[0..(getPozSt(startnd) - 1)];
    }

    public int getLen()
    {
        int i = 0;
        while (/*i < sign.Length &&*/ sign[i++] != ps.end) ;
        return i;
    }
    public Range getLast()
    {
        return pozs[getLen() - 1];
    }
    internal bool contains(ps je)

    {
        int i = 0;
        while (/*i < sign.Length &&*/ sign[i] != ps.end)
        {
            if (sign[i++] == je)
                return true;
        }
        return false;
    }
}

public enum ps : sbyte
{
    op = -1, // -n: next n symb is optional but only if there is actily this n symbols exsists
    // for example for smt {-2, word, _class, word, je};

    // "my class parser;"   = {word, _class, word, je}  =  {[word, _class,] word, je} = ok
    // "parser;"            = {word, je}                =  {[word, _class,] word, je} = ok
    // "my parser;"         = {word, word, je}          != {[word, _class,] word, je} = not ok
    
    // system symbols (can be only in the base str)
    _if = 0, // {_if, n, m} = if next n symbls are exists, the next m symbols must be in the stmt else skip m symbls
    _ifel = 1, // {_if, n, m, e} = if next n symbls are exists, the next m symbols must be in the stmt else 'e' symbls must be in the stmt
    repitable = 2, // {_if, n} = the next n symbols can be repited some times
    skip, // skip next symbols check and return true
    end, // end of stmt (must be in base and inpunt strs)

    // the actiual symbols
    word = 10, // any work
    skob_word, // (..)
    fig_skob_op, // {
    fig_skob_cl, // }
    tr_skob_word, //  <..>
    ravn, // =
    zero, // 0
    virt, // virtual
    pstatic, // static
    _const, // const
    _ref, // * or &
    export, // EXPORT
    pclass, // class
    clsepr, // :
    _using, // using
    typedef, // typedef
    template, // template
    _override, // override
    je, // ;
}

public class parser
{
    static public Dictionary<string, ps> wordparser;

    public parcInfo lineInfo;
    public parser()
    {
        wordparser = new Dictionary<string, ps>();
        wordparser.Add("virtual", ps.virt);
        wordparser.Add("satatic", ps.pstatic);
        wordparser.Add("const", ps._const);
        wordparser.Add("EXPORT", ps.export);
        wordparser.Add("class", ps.pclass);
        wordparser.Add("using", ps._using);
        wordparser.Add("typedef", ps.typedef);
        wordparser.Add("template", ps.template);
        wordparser.Add("override", ps._override);
    }

    public bool compireSigs(ps[] baseSig, ps[] inputSig)
    {
        //if (inputSig. > baseSig.Length)
        //    return false;
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


            ps baseChar = baseSig[baseInd];
            ps inpChar = inputSig[inpInd];
            Debug.Assert(inpChar != 0);

            if (baseChar == ps.skip)
                return true;

            if (baseChar == inpChar)
            {
                if (inpChar == ps.end)
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
                else
                    return false;
            }
        }
    }
    class sigconstr
    {
        List<ps> sign = new List<ps>();

        public void addOp(bool op)
        {
            if (op)
                sign.Add(0);
        }
        public void addVirtual(bool op = false)
        {
            addOp(op);
            sign.Add(ps.virt);
        }

        public void addType()
        {
            //[const] void <T> [const][*] [const]
            ps[] typePart = new ps[] {ps.op, ps._const, ps.word,ps.op, ps.tr_skob_word, ps.op, ps._const, ps.op, ps._ref, ps.op, ps._const };
            sign.AddRange(typePart);
        }

        public void addInitZero()
        {
            sign.Add(ps.ravn);
            sign.Add(ps.zero);
        }
        public void addInit(bool op)
        {
            addOp(op);
            sign.Add(ps.ravn);
            addOp(op);
            sign.Add(ps.word);
        }
        public void addje(bool op)
        {
            addOp(op);
            sign.Add(ps.je);
        }
        public void addConst(bool op)
        {
            addOp(op);
            sign.Add(ps._const);
        }
        public void addEnd()
        {
            sign.Add(ps.end);
        }
        public void addSkip()
        {
            sign.Add(ps.skip);
        }
        void add(ps sim)
        {
            sign.Add(sim);
        }
        public static ps[] getAbstructFuncpc()
        {
            //[virual] typePart  v(...) [const]  = 0;

            sigconstr cont = new sigconstr();
            cont.addVirtual(true);
            cont.addType();
            cont.add(ps.word);
            cont.add(ps.skob_word);
            cont.addConst(true);
            cont.addInitZero();
            cont.addje(false);
            cont.addEnd();
            return cont.sign.ToArray();
        }
        public static ps[] getFuncpc()
        {
            //[virual] typePart  v(...) [const] [override] [;] 

            sigconstr cont = new sigconstr();
            cont.addVirtual(true);
            cont.addType();
            cont.add(ps.word);
            cont.add(ps.skob_word);
            cont.addConst(true);
            cont.add(0);
            cont.add(ps._override);
            cont.addje(true);
            cont.addEnd();
            return cont.sign.ToArray();
        }
        public static ps[] getMemberpc()
        {
            // [const] void <T> * mem [=] [4];
            sigconstr cont = new sigconstr();
            cont.addType();
            cont.add(ps.word);
            cont.addInit(true);
            cont.addje(false);
            cont.addEnd();
            return cont.sign.ToArray();
        }
        public static ps[] getArgpc()
        {
            //type mem [=] [4]
            sigconstr cont = new sigconstr();
            cont.addType();
            cont.add(ps.word);
            cont.addInit(true);
            cont.addEnd();
            return cont.sign.ToArray();
        }
    }


    public bool isFunc()
    {
        // [virual] [const] void [<T>] [*]  v(...) [const] [override] [;]
        ps[] funcFig = sigconstr.getFuncpc();
        return compireSigs(funcFig, lineInfo.sign);
    }

    public func getFunc()
    {
        if (!isFunc()) return null;
        func funcInfo = new func();

        int curInd = 0;
        if (lineInfo.isSign(curInd, ps.virt))
        {
            funcInfo.isVirual = true;
            ++curInd;
        }

        string type = lineInfo.getFullType(ref curInd);

        funcInfo.retType = Program.getTypeId(type);

        Debug.Assert(lineInfo.isSign(curInd, ps.word));
        funcInfo.name = lineInfo.getSub(curInd++);

        Debug.Assert(lineInfo.isSign(curInd, ps.skob_word));

        string temp = lineInfo.getSub(curInd)[1..];
        temp = temp[..(temp.Length - 1)];
        string[] typeargs = temp.Split(',');

        parser argparc = new parser();
        bool tem = false;
        foreach (var _tyarg in typeargs)
        {
            string tyarg = _tyarg.Trim();
            if (tyarg.Length == 0)
                continue;

            if (tyarg == "bc::CompireStrategy& strat")
                Console.WriteLine();
            if (tem)
            {
                int temEnd = tyarg.IndexOf("*/");
                if (temEnd != -1)
                    tyarg = tyarg[(temEnd + 2)..];
            }

            argparc.getSignature(tyarg, out tem);

            argInfo inf = argparc.getArg();
            if (inf == null)
                return null;
            Debug.Assert(inf != null);
            funcInfo.args.Add(inf);

        }
        return funcInfo;
    }


    public bool isAbstructFunc()
    {
        ps[] abstFuncFig = sigconstr.getAbstructFuncpc();
        return compireSigs(abstFuncFig, lineInfo.sign);
    }
    public bool isArg()
    {
        ps[] argFig = sigconstr.getArgpc();
        return compireSigs(argFig, lineInfo.sign);
    }

    public argInfo getArg()
    {
        if (!isArg()) return null;

        return getMemberOrArg(lineInfo);
    }

    public bool isMember()
    {
        ps[] memberFig = sigconstr.getMemberpc();
        return compireSigs(memberFig, lineInfo.sign);

    }
    static argInfo getMemberOrArg(parcInfo parcInfo)
    {
        argInfo info = new argInfo();

        int curInd = 0;
        string type = parcInfo.getFullType(ref curInd);

        info.type = Program.getTypeId(type);
        info.name = parcInfo.getSub(curInd++);

        if (parcInfo.isSign(curInd, ps.ravn))
        {
            info.defaulValue = parcInfo.getSub(curInd + 1);
        }
        return info;
    }


    public argInfo getMember()
    {
        if (!isMember()) return null;

        return getMemberOrArg(lineInfo);
    }

    public bool isClass()
    {
        // class Exopr cl1 *: public class<T>*
        ps[] classSig = new ps[] { ps.pclass, ps.export, ps.word, ps.skip };
        return compireSigs(classSig, lineInfo.sign);

    }
    public string getClass()
    {
        if (!isClass()) return "";
        return lineInfo.getSub(2);
    }

    public bool isClassProto()
    {
        // class [Exopr] cl1;
        ps[] classProtoSig = new ps[] { ps.pclass, ps.op, ps.export, ps.word, ps.je, ps.end };
        return compireSigs(classProtoSig, lineInfo.sign);
    }
    public string getClassProto()
    {
        if (!isClassProto()) return "";
        return lineInfo.getSub(lineInfo.isSign(1, ps.export) ? 2 : 1);
    }

    public bool isTemplate()
    {
        // template <class t>
        ps[] tempateSig = new ps[] { ps.template, ps.tr_skob_word, ps.end };
        return compireSigs(tempateSig, lineInfo.sign);
    }

    public string getTemplate()
    {
        if (!isTemplate())
            return "";
        // template <class t>
        string lin = lineInfo.getSub(1);
        bool tem;
        parcInfo inf = getInnerSign(lin, out tem);

        Debug.Assert(inf.sign[0] == ps.pclass && inf.sign[1] == ps.word);
        return inf.getSub(1);
    }

    public bool isUsing()
    {
        // using a = b<T>*;
        ps[] usingSig = new ps[] { ps._using, ps.word, ps.ravn, ps.word, ps.op, ps.tr_skob_word, ps.op, ps._ref, ps.je, ps.end };
        return compireSigs(usingSig, lineInfo.sign);
    }

    public (string alias, string realType) getUsing()
    {
        if (!isUsing())
            return ("", "");
        // using a = b<T>*;
        Debug.Assert(lineInfo.isSign(0, ps.pclass));
        Debug.Assert(lineInfo.isSign(1, ps.word));
        Debug.Assert(lineInfo.isSign(2, ps.ravn));

        int realSt = lineInfo.getPozSt(3);
        int realEnd = this.lineInfo.getLast().Start.Value;


        string alias = lineInfo.getSub(1);
        string real = pLine[realSt..realEnd];
        return (alias, real);
    }

    public bool isTypedef()
    {
        // typedef bc::DatagridProvider < T >* bcBarImg;
        ps[] typedefSig = new ps[] { ps.typedef, ps.word, ps.op, ps.template, ps.op, ps._ref, ps.word, ps.je, ps.end };
        return compireSigs(typedefSig, lineInfo.sign);
    }

    static parcInfo getInnerSign(string line, out bool multiComment)
    {
        parcInfo inLineInf = new parcInfo();
        inLineInf.line = line;
        inLineInf.sign = new ps[30];
        inLineInf.pozs = new Range[30];

        int curl = 0;
        //bool word = false;
        int workSt = -1;
        multiComment = false;
        void closeWord(int poz, bool initNew = false)
        {
            if (workSt != -1)
            {
                string wordstr = line[workSt..poz];
                ps code;
                inLineInf.pozs[curl] = new Range(workSt, poz);
                if (wordparser.TryGetValue(wordstr, out code))
                    inLineInf.sign[curl++] = code;
                else
                    inLineInf.sign[curl++] = ps.word;

                workSt = -1;
                //word = false;

            }
            else if (initNew)
                workSt = poz;
        }
        for (int i = 0; i < line.Length; i++)
        {
            Debug.Assert(curl < 30);

            char c = line[i];

            if (c == '/' && i < line.Length && line[i + 1] == '/')
                break;

            if (c == '/' && i < line.Length && line[i + 1] == '*')
            {
                multiComment = true;
                break;
            }

            // "dssdds "
            if (c == '\"')
            {
                while (i < line.Length && line[i] != '\"')
                {
                    ++i;
                    continue;
                }
            }

            // '3' or '\n'
            if (c == '\'')
            {
                if (line[i + 1] == '\\')
                {
                    i += 2;
                }
                else
                    i += 1;
                continue;
            }

            if (c == '\t' || c == ' ' || c == '\n' || c == '\r' || c == '\n')
            {
                closeWord(i);
                continue;
            }

            if (c == ':' && line.Length > i + 1 && line[i + 1] != ':')
            {
                if (i > 0 && line[i - 1] != ':')
                {
                    inLineInf.pozs[curl] = new Range(i, i + 1);
                    inLineInf.sign[curl++] = ps.clsepr;
                    i++;
                    continue;
                }
            }

            if (c == '(')
            {
                closeWord(i);
                int st = i;
                while (i < line.Length && line[i] != ')')
                {
                    ++i;
                    continue;
                }

                Debug.Assert(i != line.Length);
                inLineInf.pozs[curl] = new Range(st, i + 1);
                inLineInf.sign[curl++] = ps.skob_word;
                continue;
            }

            if (c == '=')
            {
                closeWord(i);
                inLineInf.pozs[curl] = new Range(i, i + 1);
                inLineInf.sign[curl++] = ps.ravn;
                continue;
            }

            if (c == ';')
            {
                closeWord(i);
                inLineInf.pozs[curl] = new Range(i, i + 1);
                inLineInf.sign[curl++] = ps.je;
                continue;
            }


            if (c == '<')
            {
                if (line[i + 1] == '=')
                    return null;
                closeWord(i);
                int coitr = 1;
                ++i;
                int st = i;
                while (i < line.Length)
                {
                    if (line[i] == '>')
                    {
                        coitr--;
                        if (coitr == 0)
                            break;
                    }
                    else if (line[i] == '<')
                    {
                        coitr++;
                    }
                    ++i;
                }
                // 5 <=33 or 4 < 6
                if (i == line.Length)
                    return null;
                //Debug.Assert(i != line.Length);
                inLineInf.pozs[curl] = new Range(st, i);
                inLineInf.sign[curl++] = ps.tr_skob_word;
                continue;
            }

            if (c == '{')
            {
                closeWord(i);
                inLineInf.pozs[curl] = new Range(i, i + 1);
                inLineInf.sign[curl++] = ps.fig_skob_op;
                continue;
            }

            if (c == '}')
            {
                closeWord(i);
                inLineInf.pozs[curl] = new Range(i, i + 1);
                inLineInf.sign[curl++] = ps.fig_skob_cl;
                continue;
            }

            if (c == '*' || c == '&')
            {
                closeWord(i);
                inLineInf.pozs[curl] = new Range(i, i + 1);
                inLineInf.sign[curl++] = ps._ref;
                continue;
            }

            if (c == '0')
            {
                closeWord(i);
                inLineInf.pozs[curl] = new Range(i, i + 1);
                inLineInf.sign[curl++] = ps.zero;
                continue;
            }

            if (workSt == -1)
                workSt = i;
        }
        closeWord(line.Length);
        inLineInf.pozs[curl] = new Range(line.Length, line.Length);
        inLineInf.sign[curl++] = ps.end;
        return inLineInf;
    }
    string pLine;
    public bool getSignature(string line, out bool multiComment)
    {
        pLine = line;
        lineInfo = getInnerSign(line, out multiComment);
        return lineInfo != null;
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

    class Program
    {
        static string pathToSource;
        static List<classInfo> classes = new List<classInfo>();
        public static Dictionary<string, int> types = new Dictionary<string, int>();

        public static int getTypeId(string type, string classTemplate = "", string funcTemplate = "")
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

        static bool check(string a)
        {
            return a.Length != 0;
        }

        static bool check(func a)
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

        static void analysFile(string filepath)
        {
            StreamReader sr = new StreamReader(filepath);
            string line;
            bool avlZone = false;
            bool startClass = false;

            classInfo classInfo = null;

            parser parser = new parser();
            int skobs = 0;
            int funSkob = -1, classSkob = -1;
            bool multiComm = false;


            void checkSkobsFnd(ps neededSkob, ref int initOfFnd)
            {
                ps curTe = parser.lineInfo.getSign(0);
                int inde = 1;
                int sd = skobs;
                while (curTe != ps.end)
                {
                    if (curTe == neededSkob)
                        initOfFnd = skobs + 1;
                    if (curTe == ps.fig_skob_op)
                        skobs++;
                    else if (curTe == ps.fig_skob_cl)
                        skobs--;
                    curTe = parser.lineInfo.getSign(inde++);
                }

                if (skobs == sd)
                    return;
                if (classSkob != -1)
                {
                    if (skobs < classSkob)
                    {
                        classSkob = -1;
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
                while (curTe != ps.end)
                {
                    if (curTe == ps.fig_skob_op)
                        skobs++;
                    else if (curTe == ps.fig_skob_cl)
                        skobs--;
                    curTe = parser.lineInfo.getSign(inde++);
                }

                if (classSkob != -1)
                {
                    if (skobs < classSkob)
                    {
                        classSkob = -1;
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

                if (line == "class EXPORT Baritem : public Barbase<T>")
                    Console.WriteLine();
                if (!parser.getSignature(line, out multiComm))
                    continue;

                // TEMPLATE
                string temp = parser.getTemplate();
                if (check(temp))
                {
                    //Debug.Assert(templ.Length == 0);
                    templ = temp;
                    continue;
                }

                string clas = parser.getClassProto();
                if (check(clas))
                {
                    templ = "";
                    getTypeId(clas);
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
                    getTypeId(clas);

                    classes.Add(classInfo);

                    avlZone = false;
                    //skobs = 0;
                    if (check(templ))
                    {
                        classInfo.tempType = templ;
                        templ = "";
                    }

                    checkSkobsFnd(ps.pclass, ref classSkob);
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

                    if (avlZone && (funSkob == -1))
                    {
                        var membr = parser.getMember();
                        if (check(membr))
                        {
                            int mem = 0;
                            classInfo.members.Add(membr);
                            checkSkobs();

                            continue;
                        }

                        //"bc::BarRoot<T>* getRootNode()"
                        var func = parser.getFunc();
                        if (check(func))
                        {
                            if (check(templ))
                            {
                                func.tempType = templ;
                                templ = "";
                            }

                            classInfo.funcs.Add(func);
                            if (parser.lineInfo.contains(ps.je))
                                checkSkobs();
                            else
                                checkSkobsFnd(ps.skob_word, ref funSkob);
                            continue;
                        }
                    }
                }

                checkSkobs();
            }
        }

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


        static string getTypeFromKey(int key)
        {
            foreach (var l in types)
            {
                if (l.Value == key)
                    return l.Key;
            }
            return "";
        }
        static void Main(string[] args)
        {
            computePath();
            types.Add("invalid", 0);

            foreach (var filename in Directory.GetFiles(pathToSource))
            {
                string filepath = Path.Combine(pathToSource, filename);
                analysFile(filepath);
            }

            foreach (var cla in classes)
            {
                if (cla.tempType?.Length != 0)
                {
                    Console.WriteLine("template<class {0}>", cla.tempType);
                }
                Console.WriteLine("class EXPORT {0}", cla.name);
                Console.WriteLine("{");


                string getArg(argInfo info)
                {
                    string ret = getTypeFromKey(info.type) + " " + info.name;
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

                    ret += getTypeFromKey(fun.retType);

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
            Console.WriteLine("Hello World!");
        }
    }
}