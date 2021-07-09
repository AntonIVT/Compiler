#include "include/tree.hpp"
#include "include/parser.hpp"
#include "include/translator.hpp"
#include "include/compiler.hpp"
#include "include/dump_tree.hpp"

int FileSize(FILE *file)
{
    assert(file != NULL);

	int _file_size = -1;

	fseek(file, 0, SEEK_END);
	_file_size = ftell(file);

	return _file_size;
}

int main(int argc, char **argv)
{
    const char *input_file  = nullptr;
    const char *output_file = nullptr;

    /* Compile with dumping program graph (necessary dot) */
    bool isGraph = false;
    /* Loading program from AST-file */
    bool isLoad = false;
    /* Compile with assembly */
    bool isAsm  = false;
    
    if (argc == 1)
    {
        printf("No input file\n");
        return 1;
    }
    
    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-graph"))
        {
            isGraph = true;
            continue;
        }
        if (!strcmp(argv[i], "-load"))
        {
            isLoad = true;
            continue;
        }
        if (!strcmp(argv[i], "-asm"))
        {
            isAsm = true;
            continue;
        }
        if (!strcmp(argv[i], "-o"))
        {
            if (i + 1 == argc)
            {
                printf("No output file\n");
                return 1;
            }
            output_file = argv[i + 1];
            i++;
            continue;
        }
        input_file = argv[i];
    }

    if (input_file == nullptr)
    {
        printf("No input file\n");
        return 1;
    }
        
    FILE *input = fopen(input_file, "rb");
    assert(input != nullptr);
    
    int size = FileSize(input);
    fseek(input, 0, SEEK_SET);
    
    char * buffer = (char *)calloc(size + 1, sizeof(char));
    fread(buffer, sizeof(char), size, input);
    fclose(input);
    
    if (isLoad)
    {
        Tree_reader reader = {};
        reader.construct();
        Tree tree = {};
        tree = reader.get_tree(buffer);
        reader.destruct();
        
        if (isGraph)
            tree.Dump("AST-graph.pdf");
        
        if (output_file == nullptr)
            output_file = "program.doors";

        Code_generator generator = {};
        generator.construct(output_file);
        generator.generate(&tree);
        generator.destruct();
        
        tree.destruct();
        free(buffer);

        return 0;
    }
    
    Parser parser = {};
    parser.construct(buffer);
    
    Tree tree = {};
    tree.construct();
    
    tree.root = parser.get_FuncCompaund();
    free(buffer);
    if (tree.root == nullptr)
        return 1;
    
    parser.destruct();
    
    if (isGraph)
    {
        tree.Dump("AST-graph.pdf");
    
        Tree_dumper dumper = {};
        dumper.construct(&tree, "AST.txt");
        dumper.dump_tree();
        dumper.destruct();
    }

    if (isAsm)
    {
        ProgramTranslator translator = {};
        translator.construct("program.asm", &tree);
        translator.translate_program();
        translator.destruct();
    }

    ProgramCompiler compiler = {};
    compiler.construct(&tree);
    if (output_file == nullptr)
        output_file = "program.out";
    compiler.compile_program(output_file, false);
    compiler.destruct();
    
    tree.destruct();
}
