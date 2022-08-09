using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Parcsh
{
    public class CWrapper
    {

        static List<classInfo> classes = new List<classInfo>();
        public static Dictionary<string, int> types = new Dictionary<string, int>();

        static string getTypeFromKey(int key)
        {
            foreach (var l in types)
            {
                if (l.Value == key)
                    return l.Key;
            }
            return "";
        }

        static string getArg(argInfo info)
        {
            // TODO replace class names with handlers
            string ret = getTypeFromKey(info.type) + " " + info.name;
            if (info.defaulValue.Length != 0)
                ret += " = " + info.defaulValue;
            return ret;
        }

        static string createClassConstrProto(Constr fun, classInfo cla)
        {
            string handlName = cla.name + "_handler";

            string ret = "";
            ret += handlName + " create_" + cla.name + "(";
            foreach (var arg in fun.args)
                ret += getArg(arg) + ", ";

            if (fun.args.Count != 0)
                ret = ret[..(ret.Length - 2)];
            ret += ")";

            return ret;
        }

        static string createClassConstrImpl(Constr fun, classInfo cla)
        {
            string ret = "\t{";
            ret += "int handl = " + cla.name + "_counter++;\n";
            ret += "\t\t" + cla.name + "_map.insert(std::pair(handl, new " + cla.name+ "(";
            foreach (var arg in fun.args)
                ret += arg.name + ", ";

            ret += ")\n";
            ret += "\t\treturn handl;";
            ret += "\t}";

            return ret;
        }

        static string createClassDestrProto(classInfo cla)
        {
            string handlName = cla.name + "_handler";
            return"\tvoid destroy_" + cla.name + "(" + handlName + " handle)";
        }

        static string createClassDestrImpl(classInfo cla)
        {
            string ret = "\t{";
            ret += "\t\tdelete " + cla.name + "_map[handle]\n";
            ret += "\t}";

            return ret;
        }

        // static string createClassMemberGetter(classInfo cla)
        // {
        //     return"\tvoid destroy_" + cla.name + "()";
        // }
        static string createClassMethodProto(func fun, classInfo cla)
        {
            string handlName = cla.name + "_handler";

            string ret = "";
            ret += getTypeFromKey(fun.retType);

            ret += " " + fun.name + "(" + handlName + " handler, ";

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

            return ret;
        }

        static string createClassMethodImpl(func fun, classInfo cla)
        {
            string ret = "\t{";
            ret += "\t\t" + cla.name + "_map[handle]->" + fun.name + "(";
            foreach (var arg in fun.args)
            {
                ret += arg.name + ", ";
            }
            ret = ret[..(ret.Length - 2)];
            ret += "\t}";

            return ret;
        }

        static void printWrapperClasses()
        {
            foreach (var cla in classes)
            {
                Console.WriteLine("std::unordered_map<int,{0}*> {0}_map;", cla.name);
                Console.WriteLine("int {0}_counter;", cla.name);
            }
            Console.WriteLine("extern \"C\"");
            Console.WriteLine("{");

            foreach (var cla in classes)
            {
                Console.WriteLine("\t// For class : {0}\n", cla.name);
                string handlName = cla.name + "_handler";
                Console.WriteLine("\ttypedef int {0}", handlName);

                Console.WriteLine("\t// Constructors:");

                foreach (var fun in cla.consrts)
                {
                    Console.WriteLine("\t{0};", createClassConstrProto(fun, cla));
                    Console.WriteLine("");
                }

                Console.WriteLine("\t// Desructor:");

                if (cla.destructor != null)
                {
                    Console.WriteLine("\t{0};", createClassDestrProto(cla));
                }
                Console.WriteLine("");


                // Console.WriteLine("\t// Members:");

                // foreach (var memb in cla.members)
                // {
                //     string ret = "\t" + getArg(memb) + ";";
                //     Console.WriteLine(ret);
                //     Console.WriteLine("");
                // }

                Console.WriteLine("\t// Functions:");

                foreach (var fun in cla.funcs)
                {
                    string ret = "\t" + createClassMethodProto(fun, cla);
                    ret += ";";
                    Console.WriteLine(ret);
                }
                Console.WriteLine("}");
                Console.WriteLine("");
                Console.WriteLine("");

            }
        }
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
            public static extern IntPtr Barbase_byte_removePorog(IntPtr self, byte porog);
            [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
            public static extern IntPtr Barbase_byte_preprocessBar(IntPtr self, byte porog, bool normalize);
            [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
            public static extern IntPtr Barbase_byte_compireFull(IntPtr self, IntPtr Y, CompireStrategy strat, out float returnValue);
            [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
            public static extern IntPtr Barbase_byte_clone(IntPtr self, out IntPtr returnValue);
            [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
            public static extern IntPtr Barbase_byte_sum(IntPtr self, out byte returnValue);
            [Pure, DllImport(DllExtern, CallingConvention = CallingConvention.Cdecl, ExactSpelling = true)]
            public static extern IntPtr Barbase_byte_relen(IntPtr self);
        }
    }*/


}