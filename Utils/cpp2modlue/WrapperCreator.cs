using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Parcsh
{
    public class CWrapperPrinter : CppReader
    {
        struct Handler
        {
            public string baseName;
            public string handlerName;
            public bool hasMap = false;

            public Handler(string baseName, string handlerName)
            {
                this.baseName = baseName;
                this.handlerName = handlerName;
                this.hasMap = true;
            }
            public Handler(string baseName)
            {
                this.baseName = baseName;
                this.handlerName = "";
                this.hasMap = false;
            }
        }
        Dictionary<int, Handler> handlerMap = new();

        string getBaseTypeFromKey(int key)
        {

            foreach (var l in parser.types)
            {
                if (l.Value == key)
                    return l.Key;
            }
            return "";
        }


        string getTypeFromKey(int key)
        {
            if (handlerMap.ContainsKey(key))
            {
                return handlerMap[key].handlerName;
            }

            return getBaseTypeFromKey(key);
        }


        protected string getArgWithHandler(argInfo info)
        {
            string ret = getTypeFromKey(info.type) + " " + info.name;
            if (info.defaulValue.Length != 0)
                ret += " = " + info.defaulValue;
            return ret;
        }

        protected string getArgOrMap(argInfo arg)
        {
            if (handlerMap.ContainsKey(arg.type) && handlerMap[arg.type].hasMap)
            {
                return buildMap(handlerMap[arg.type].baseName) + "[" + arg.name + "]";
            }
            else
                return arg.name;
        }

        StreamWriter outFile;
        void printLine(string str)
        {
            outFile.WriteLine(str);
        }
        void printLine(string str, params object?[]? args)
        {
            outFile.WriteLine(str, args);
        }

        string createClassConstrProto(Constr fun, classInfo cla)
        {
            string handlName = buildHandler(cla.name);

            string ret = "";
            ret += handlName + " create_" + cla.name + "(";
            foreach (var arg in fun.args)
                ret += getArgWithHandler(arg) + ", ";

            if (fun.args.Count != 0)
                ret = ret[..(ret.Length - 2)];
            ret += ")";

            return ret;
        }

        string buildHandler(string name)
        {
            return name + "Handler";
        }

        string buildCounter(string name)
        {
            return name + "Counter";
        }

        string buildMap(string name)
        {
            return name + "Map";
        }


        string createClassConstrImpl(Constr fun, classInfo cla)
        {
            string ret = "{\n";
            ret += "\t\tint handl = " + buildCounter(cla.name) + "++;\n";
            ret += "\t\t" + buildMap(cla.name) + ".insert(std::pair(handl, new " + cla.name + "(";
            foreach (var arg in fun.args)
                ret += arg.name + ", ";

            if (fun.args.Count != 0)
                ret = ret[..(ret.Length - 2)];
            ret += ")));\n";

            ret += "\t\treturn handl;\n";
            ret += "\t}";

            return ret;
        }

        string createClassDestrProto(classInfo cla)
        {
            string handlName = buildHandler(cla.name);
            return "void destroy_" + cla.name + "(" + handlName + " handler)";
        }

        string createClassDestrImpl(classInfo cla)
        {
            string ret = "{\n";
            ret += "\t\tdelete " + buildMap(cla.name) + "[handler];\n";
            ret += "\t}";

            return ret;
        }

        // static string createClassMemberGetter(classInfo cla)
        // {
        //     return"\tvoid destroy_" + cla.name + "()";
        // }
        string createClassMethodProto(func fun, classInfo cla)
        {
            string handlName = buildHandler(cla.name);

            string ret = "";
            ret += getTypeFromKey(fun.retType);

            ret += " " + cla.name + "_" + fun.name + "(" + handlName + " handler, ";

            foreach (var arg in fun.args)
            {
                ret += getArgWithHandler(arg) + ", ";
            }

            ret = ret[..(ret.Length - 2)];
            ret += ")";

            if (fun.isConst)
            {
                ret += " const";
            }

            return ret;
        }

        string createClassMethodImpl(func fun, classInfo cla)
        {
            bool withRetVar = false;
            string ret = "{\n";
            HashSet<string> safew = new HashSet<string>();// { "Barscalar", "point", "barvalue", "BarRect" };
            string basneTypeName = getBaseTypeFromKey(fun.retType);
            if (basneTypeName != "void")
            {
                fun.retPtr = !safew.Contains(basneTypeName);
                if (handlerMap.ContainsKey(fun.retType) && handlerMap[fun.retType].hasMap)
                {
                    ret += "\t\t" + basneTypeName + (fun.retPtr ? "* ret = " : " ret = ");
                    withRetVar = true;
                }
                else
                {
                    ret += "\t\treturn ";
                }
            }
            else
                ret += "\t\t";

            ret += buildMap(cla.name) + "[handler]->" + fun.name + "(";

            foreach (var arg in fun.args)
                ret += getArgOrMap(arg) + ", ";

            if (fun.args.Count != 0)
                ret = ret[..(ret.Length - 2)];
            ret += ");\n";

            if (withRetVar)
            {
                string mapName = buildMap(handlerMap[fun.retType].baseName);

                ret += "\t\tunsigned int handl = " + buildCounter(cla.name) + "++;\n";
                ret += "\t\t" + mapName + ".insert(pair(handl, ret));\n";
                ret += "\t\treturn handl;\n";
            }
            ret += "\t}";

            return ret;
        }


        public void printWrapperClasses()
        {
            HashSet<string> blacklist = new() { "BarscalHash", "DatagridProvider", "BarImg", "pointHash" };

            Dictionary<string, HashSet<string>> backMethdos = new();
            backMethdos.Add("Baritem", new HashSet<string> { "getRootNode", "getCvMat", "getCvRect", "getBarcode3dValue" });
            backMethdos.Add("barline", new HashSet<string> { "getMatrix", "getEnclusivePoints", "getExclusivePoints", "getBarcode3d", "getPoints", "getCvMat", "getCvRect", "getBarcode3dValue","getBarcode3dSize", "getPointsInDict", "getRect", "getChildren" });
            backMethdos.Add("Barscalar", new HashSet<string> { "text", "toCvVec", "value", "operator>", "operator+", "operator-", "operator/", "operator[]" });

            string s = "D:\\Programs\\C++\\Barcode\\PrjCWrapper\\gen\\c_wrapper.h";
            outFile = new StreamWriter(s);

            printLine("extern \"C\"");
            printLine("{");
            printLine("\t// Class handlers");

            //int r = parser.types["Barbase"];
            //handlerMap.Add(r, new Handler("BarHandler"));
            int r = parser.types["size_t"];
            handlerMap.Add(r, new Handler("size_t", "unsigned long long"));
            //r = parser.types["uchar"];
            //handlerMap.Add(r, new Handler("unsigned char"));

            foreach (var cla in classes)
            {
                if (blacklist.Contains(cla.name))
                    continue;

                string handlName = buildHandler(cla.name);

                int vv;
                if (parser.types.TryGetValue(cla.name, out vv))
                {
                    handlerMap.Add(vv, new Handler(cla.name, handlName));
                }
                printLine("\ttypedef unsigned int {0};", handlName);
            }
            //printLine("\ttypedef unsigned int BarHandler;");

            printLine("\n");
            blacklist.Add("Barbase");

            foreach (var cla in classes)
            {
                if (blacklist.Contains(cla.name))
                    continue;

                printLine("\t// For class : {0}\n", cla.name);

                printLine("\t// Constructors:");

                foreach (var fun in cla.consrts)
                {
                    printLine("\t{0};", createClassConstrProto(fun, cla));
                    printLine("");
                }

                printLine("\t// Desructor:");

                if (cla.destructor != null)
                {
                    printLine("\t{0};", createClassDestrProto(cla));
                }
                printLine("");


                //printLine("\t// Members:");

                // foreach (var memb in cla.members)
                // {
                //     string ret = "\t" + getArg(memb) + ";";
                //    printLine(ret);
                //    printLine("");
                // }


                HashSet<string> skl;
                if (!backMethdos.TryGetValue(cla.name, out skl))
                    skl = new HashSet<string>();

                printLine("\t// Functions:");

                foreach (var fun in cla.funcs)
                {
                    if (skl.Contains(fun.name))
                        continue;

                    if (fun.name == "count")
                        Console.WriteLine();
                    printLine("\t{0};", createClassMethodProto(fun, cla));
                }
            }
            printLine("}");
            printLine("");

            foreach (var cla in classes)
            {
                if (blacklist.Contains(cla.name))
                    continue;
                if (cla.name == "Barscalar")
                {
                    printLine("barmap<unsigned int, {0}> {0}Map;", cla.name);
                }
                else
                    printLine("barmap<unsigned int, bc::{0}*> {0}Map;", cla.name);
                printLine("unsigned int {0}Counter;", cla.name);
            }

            outFile.Close();
            s = "D:\\Programs\\C++\\Barcode\\PrjCWrapper\\gen\\c_wrapper.cpp";
            outFile = new StreamWriter(s);

            printLine("#include \"c_wrapper.h\"\n");
            printLine("#include \"barcodeCreator.h\"\n");
            printLine("using namespace std;");
            printLine("using namespace bc;");
            printLine("");
            printLine("extern \"C\"");
            printLine("{");

            foreach (var cla in classes)
            {
                if (blacklist.Contains(cla.name))
                    continue;

                printLine("\t// For class : {0}\n", cla.name);
                printLine("\t// Constructors:");

                foreach (var fun in cla.consrts)
                {
                    printLine("\t{0}", createClassConstrProto(fun, cla));
                    printLine("\t{0}", createClassConstrImpl(fun, cla));
                    printLine("");
                }

                printLine("\t// Desructor:");

                if (cla.destructor != null)
                {
                    printLine("\t{0}", createClassDestrProto(cla));
                    printLine("\t{0}", createClassDestrImpl(cla));
                }
                printLine("");


                //printLine("\t// Members:");

                // foreach (var memb in cla.members)
                // {
                //     string ret = "\t" + getArg(memb) + ";";
                //    printLine(ret);
                //    printLine("");
                // }

                HashSet<string> skl;
                if (!backMethdos.TryGetValue(cla.name, out skl))
                    skl = new HashSet<string>();

                printLine("\t// Functions:");

                foreach (var fun in cla.funcs)
                {
                    if (skl.Contains(fun.name))
                        continue;

                    printLine("\t{0}", createClassMethodProto(fun, cla));
                    printLine("\t{0}", createClassMethodImpl(fun, cla));
                }
            }
            printLine("}");
            printLine("");

            outFile.Close();
        }
    }



    //        public void printCClasses()
    //        {
    //            string getFirstDict = @"

    //void getFirstNotNull(void* dict, int maxSize)
    //{
    //    for(int i=0;i<maxSize;++i)
    //        if (dict[i]==0)
    //            return i;
    //}

    //void deleteFromDict(void* dict, int handler)
    //{
    //    dict[handler] = 0;
    //}
    //";

    //            // TEPLATE
    //            string[] templates = { "uchar", "float", "short" };
    //            StringBuilder enumBuilder = new StringBuilder("enum bartype { ");
    //            foreach (var t in templates)
    //            {
    //                enumBuilder.Append(t + ", ");
    //            }
    //            enumBuilder[enumBuilder.Length - 2] = '}';
    //            enumBuilder[enumBuilder.Length - 1] = ';';


    //            string argClassTmp = "bartype classType";
    //            string argFuncTmp = "bartype funcType";

    //            string dicts = "";
    //            void addClassToDict(string name, string temlate = "")
    //            {
    //                if (temlate?.Length != 0)
    //                {
    //                    foreach (var t in templates)
    //                    {
    //                        dicts += name + "<" + t + "> * " + name + "_" + t + "_dict[100];\n";
    //                    }
    //                }
    //                else
    //                    dicts += name + "* " + name + "dict[100];\n";
    //            }

    //            string buildConstructorWrapper(string type, string fullArgs, string args, string temlate = "")
    //            {
    //                string prefix = "bar_";
    //                string impl = "\tunsigned int create_" + type + temlate + fullArgs + "\n\t{\n";
    //                addClassToDict(type, temlate);
    //                if (temlate?.Length != 0)
    //                {
    //                    foreach (var t in templates)
    //                    {
    //                        string dictName = type + "_" + t + "_dict";
    //                        impl +=
    //                            $"\n\t\tif (classType== bartype::{prefix + t})" +
    //                             "\n\t\t{" +
    //                            $"\n\t\t\tint handler = getFirstNotNull({dictName},100);" +
    //                            $"\n\t\t\t{dictName}[handler] = new {type}<{t}>{args};" +
    //                            $"\n\t\t\treturn handler;" +
    //                             "\n\t\t}\n";
    //                    }
    //                    impl += "\t}\n";
    //                    //        int handled = getFirstNotNull(CLASSdict,100);
    //                }
    //                else
    //                {
    //                    string dictName = type + "_dict";

    //                    impl += "\t{" +
    //                            $"\n\t\tint handler = getFirstNotNull({dictName},100);" +
    //                            $"\n\t\t{dictName} = new {type}{args};" +
    //                            $"\n\t\treturn handler;" +
    //                             "\n\t}\n";
    //                }

    //                return impl;
    //            }
    //            //            @"
    //            //if (type==bartype::bar_uchar)
    //            //    {
    //            //        int handled = getFirstNotNull(CLASSdict,100);
    //            //        CLASSdict[handler] = new CLASS<uchar>(...);
    //            //        return handler;
    //            //}
    //            //";
    //            string[] voider = { "" };

    //           printLine("extern \"C\" \n{");
    //            //Console.WriteLine("// Classes");
    //            //foreach (var cla in classes)
    //            //{
    //            //   printLine("\t{0}", cla.);
    //            //}

    //            foreach (var cla in classes)
    //            {
    //                string[] inpls = null;

    //               printLine("// CLASS {0}", cla.name);


    //               printLine("\t// Constructors:");
    //                foreach (var fun in cla.consrts)
    //                {
    //                    string ret = "";
    //                    string realAgrs = "(";

    //                    int added = 0;
    //                    if (cla.tempType?.Length != 0)
    //                    {
    //                        ret += argClassTmp + ", ";
    //                        ++added;
    //                    }
    //                    if (fun.tempType?.Length != 0)
    //                    {
    //                        ret += argFuncTmp + ", ";
    //                        ++added;
    //                    }

    //                    foreach (var arg in fun.args)
    //                    {
    //                        ret += getArg(arg) + ", ";
    //                        realAgrs += getArg(arg) + ", ";

    //                        ++added;
    //                    }
    //                    if (added != 0)
    //                        ret = ret[..(ret.Length - 2)];
    //                    ret += ")";

    //                    if (fun.args.Count != 0)
    //                        realAgrs = realAgrs[..(realAgrs.Length - 2)];
    //                    realAgrs += ")";


    //                   printLine(buildConstructorWrapper(cla.name, ret, realAgrs, cla.tempType));
    //                   printLine("");
    //                }


    //               printLine("");
    //               printLine("");
    //            }
    //           printLine(getFirstDict);
    //           printLine(dicts);
    //           printLine("}");

    //        }

}

/*
namespace BarcodeNet.Internal.PInvoke
{
    [SourceFile(@"C:\Users\undefined\Documents\Barcode-main\PrjBarlib\include\barclasses.h")]
    public static partial class NativeMethods
    {
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_self_destructor(IntPtr self);
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_removeByThreshold(IntPtr self, byte porog);
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_preprocessBarcode(IntPtr self, byte porog, bool normalize);
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_compareFull(IntPtr self, IntPtr Y, CompareStrategy strat, out float returnValue);
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_clone(IntPtr self, out IntPtr returnValue);
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_sum(IntPtr self, out byte returnValue);
        [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
        public static extern IntPtr Barbase_byte_relength(IntPtr self);
    }
}*/
