#pragma once

#include "tree.hpp"
#include <cstring>
#include "list.hpp"

struct Tree_dumper
{
    Tree *tree;
    FILE* dump;
    int count;
    List<Node *> nodes_list;
    int curr_number;
    
    void construct(Tree *tree, const char *dump_file)
    {
        assert(dump_file != nullptr && tree != nullptr);
        
        this->tree = tree;
        dump = fopen(dump_file, "wb");
        count = 1;
        curr_number = 0;
        assert(dump != nullptr);
        nodes_list.construct(1);
    } 
    
    void destruct()
    {
        fclose(dump);
        nodes_list.destruct();
    }
    
    #define PRINT(operator) fprintf(dump, operator);
    
    void dump_tree()
    {
        assert(tree != nullptr && dump != nullptr);
        
        get_count(tree->root);
        
        fprintf(dump,"%i\n" 
                     "None @ 0 0\n", count);
        curr_number++;
        
        if (tree->root == nullptr)
            return;
        
        nodes_list.push_back(tree->root); 
        
        while(nodes_list.get_size() != 0)
        {
            Node* tmp = nodes_list.get(nodes_list.begin());
            
            switch (tmp->type)
            {
            case NUM:
                fprintf(dump, "Number %lg", tmp->value.dval);
                break;
            case IDENT:
                fprintf(dump, "Identifier \"%s\"", tmp->value.strval);
                break;
            case BIN_OPER:
                fprintf(dump, "Operator");
                switch (tmp->value.ival)
                {
                case ADD:
                    PRINT(" \"+\"")
                    break;
                case SUB:
                    PRINT(" \"-\"")
                    break;
                case MUL:
                    PRINT(" \"*\"")
                    break;
                case DIV:
                    PRINT(" \"/\"")
                    break;
                case LESS:
                    PRINT(" \"<\"")
                    break;
                case GREAT:
                    PRINT(" \">\"")
                    break;
                case LEQ:
                    PRINT(" \"<=\"")
                    break;
                case GEQ:
                    PRINT(" \">=\"")
                    break;
                case EQ:
                    PRINT(" \"==\"")              
                    break;
                case NEQ:
                    PRINT(" \"!=\"")
                    break;
                case OR:
                    PRINT(" \"||\"")
                    break;
                case AND:
                    PRINT(" \"&&\"")
                    break;
                default:
                    printf("Unknown binary operation\n");
                    break;
                }
            break;
            case VAR_DECL:
                fprintf(dump, "VarDef \"%s\"", tmp->value.strval);
                break;
            case FUN_DECL:
                fprintf(dump, "FuncDecl \"%s\"", tmp->value.strval);
                break;
            case FUN_CALL:
                fprintf(dump, "FuncCall \"%s\"", tmp->value.strval);
                break;
            case LINK:
                PRINT("Link @");
                break;
            case INPUT:
                PRINT("Input @");
                break;
            case PRINT:
                PRINT("Print @");
                break;
            case COND_ST:
                PRINT("IfStmt @");
                break;
            case WHILE_ST:
                PRINT("WhileStmt @")
                break;
            case ASSGN_ST:
                PRINT("AssignExpr")
                switch (tmp->value.ival)
                {
                case ASSGN:
                    PRINT(" \"=\"")
                    break;
                case ASSGN_A:
                    PRINT(" \"+=\"")
                    break;
                case ASSGN_S:
                    PRINT(" \"-=\"")
                    break;
                case ASSGN_M:
                    PRINT(" \"*=\"")
                    break;
                case ASSGN_D:
                    PRINT(" \"/=\"")
                    break;
                default:
                    break;
                }
                break;
            case RET_ST:
                fprintf(dump, "ReturnStmt @");
                break;        
            default:
                break;
            }
            
            if (tmp->lnode != nullptr)
            {
                nodes_list.push_back(tmp->lnode);
                curr_number++;
                fprintf(dump, " %i", curr_number);
            }
            else
                fprintf(dump, " 0");
                
            if (tmp->rnode != nullptr)
            {
                nodes_list.push_back(tmp->rnode);
                curr_number++;
                fprintf(dump, " %i", curr_number);
            }
            else
                fprintf(dump, " 0");
                
            fprintf(dump, "\n");
            
            nodes_list.pop_front();
        }
    }
    
    #undef PRINT
    
    void get_count(Node* node)
    {
        if (node == nullptr) return;
           
        count++;
        get_count(node->lnode);
        get_count(node->rnode);
    }
};

struct LabelReader
{
    Node *node_ptr;
    int left;
    int right;
};

struct Tree_reader
{
    LabelReader *label_array;
    int count;
    const char* input;
    const char curr_node[100] = "";
    
    void construct()
    {   
        label_array = nullptr;
        count = 0;
        input = nullptr;
    }
    
    Tree get_tree(const char *file)
    {
        assert(file != nullptr);
        
        input = file;
        sscanf(input, "%i ", &count);
        
        label_array = (LabelReader *)calloc(count, sizeof(LabelReader));
        input = strchr(input, '\n');
        input++;
        
        label_array[0].node_ptr = nullptr;
        
        input = strchr(input, '\n');
        input++;
                
        for (int i = 1; i < count; i++)
        {
            label_array[i] = get_label();
        }
        
        for (int i = 1; i < count; i++)
        {
            label_array[i].node_ptr->lnode = label_array[label_array[i].left].node_ptr;
            label_array[i].node_ptr->rnode = label_array[label_array[i].right].node_ptr;
        }
        
        Tree tree = {};
        tree.construct(label_array[1].node_ptr);
        return tree;
    }
    
    LabelReader get_label()
    {
        LabelReader label = {};
        label.node_ptr = (Node *)calloc(1, sizeof(Node));
        int tmp = 0;
                        
        for (int i = 0; i < sizeof(names_types) / sizeof(names_types[0]); i++)
        {
            if (input == strstr(input, names_types[i].name))
            {
                label.node_ptr->type = names_types[i].type;
                input += strlen(names_types[i].name) + 1;
                break;
            }
        }
        
        switch (label.node_ptr->type)
        {
        case NUM:
            sscanf(input, "%lg %n", &label.node_ptr->value.dval, &tmp);
            input += tmp;
            break;
        case IDENT: case FUN_DECL:
        case FUN_CALL: case VAR_DECL:
        {
            input = strchr(input, '\"') + 1;
            char *kav = (char*)strchr(input, '\"');
            assert(kav != nullptr);
            *kav = 0;
            
            label.node_ptr->value.strval = strdup(input);
            input = kav + 2;
            break;
        }
        case BIN_OPER:
        {
            input = strchr(input, '\"') + 1;
            char *kav = (char*)strchr(input, '\"');
            assert(kav != nullptr);
            *kav = 0;
            
            for (int i = 0; i < sizeof(names_oper) / sizeof(names_oper[0]); i++)
            {
                if (!strcmp(names_oper[i].name, input))
                {
                    label.node_ptr->value.ival = names_oper[i].oper;
                    input = kav + 2;
                    break;
                }
            }
            break;
        }
        case ASSGN_ST:
        {
            input = strchr(input, '\"') + 1;
            char *kav = (char*)strchr(input, '\"');
            assert(kav != nullptr);
            *kav = 0;
            
            for (int i = 0; i < sizeof(names_assgn) / sizeof(names_assgn[0]); i++)
            {
                if (!strcmp(names_assgn[i].name, input))
                {
                    label.node_ptr->value.ival = names_assgn[i].oper;
                    input = kav + 2;
                    break;
                }
            }
            break;
        }
        default:
            input = (char *)strchr(input, '@') + 2;
            break;
        }
        
        sscanf(input, "%i %n", &(label.left), &tmp);
        input += tmp;
        sscanf(input, "%i %n", &(label.right), &tmp);
        input += tmp;
        
        return label;
    }
    
    void destruct()
    {
        if (label_array != nullptr)
            free(label_array);
    }
};

struct Code_generator
{
    FILE* output;
    int curr_col;
    const char* comma;
    const char* lbrack;
    const char* rbrack;
    const char* lcomp_brack;
    const char* rcomp_brack;
    const char* end;
    const char* _else;
     
    void construct(const char *output_file)
    {
        assert(output_file != nullptr);
        output = fopen(output_file, "wb");
        assert(output != nullptr);
                
        for (int i = 0; i < sizeof(words_list) / sizeof(words_list[0]); i++)
        {
            if (words_list[i].num == COMMA_K)
                comma = words_list[i].word;
            else if (words_list[i].num == LBRACK_K)
                lbrack = words_list[i].word;
            else if (words_list[i].num == RBRACK_K)
                rbrack = words_list[i].word;
            else if (words_list[i].num == LCOMPBRACK_K)
                lcomp_brack = words_list[i].word;
            else if (words_list[i].num == RCOMPBRACK_K)
                rcomp_brack = words_list[i].word;
            else if (words_list[i].num == END_K)
                end = words_list[i].word;
            else if (words_list[i].num == ELSE_K)
                _else = words_list[i].word;
        }
        
    }
    
    void generate(Tree *tree)
    {
        assert(tree != nullptr);
        if (tree->root == nullptr) return;
        
        curr_col = 0;
        
        generate_node(tree->root, 3);
    }
    
    #define PRINT(operator) fprintf(output, operator);
    
    // link 1 == expr comp
    // link 2 == func args
    // link 3 == functions  
    
    void generate_node(Node *node, int link_type = 0)
    {
        if (node == nullptr) return;
                
        const char *key_word = nullptr;
        
        switch (node->type)
        {
        case BIN_OPER: case ASSGN_ST:
            
            for (int i = 0; i < sizeof(words_list) / sizeof(words_list[0]);i++)
            {
                if (words_list[i].num == node->value.ival)
                {
                    key_word = words_list[i].word;
                    break;
                }
            }
            break;
        case VAR_DECL: case FUN_DECL:
        case COND_ST: case WHILE_ST:
        case RET_ST: case INPUT:
        case PRINT:
            for (int i = 0; i < sizeof(words_list) / sizeof(words_list[0]);i++)
            {
                if (words_list[i].type == node->type)
                {
                    key_word = words_list[i].word;
                    break;
                }
            }
            break;
        }
        
        switch (node->type)
        {
        case NUM:
            fprintf(output, " %lg", node->value.dval);
            break;
        case IDENT:
            fprintf(output, " %s", node->value.strval);
            break;
        case BIN_OPER: case ASSGN_ST:
        
            if (node->rnode == nullptr)
            {
                PRINT("-(");
                generate_node(node->lnode);
                PRINT(")");
                break;
            }
            
            if (node->type == BIN_OPER)
            {
                if (node->lnode->type != NUM && node->lnode->type != IDENT)
                    fprintf(output, "%s", lbrack);
            }
            generate_node(node->lnode);
            if (node->type == BIN_OPER)
            {
                if (node->lnode->type != NUM && node->lnode->type != IDENT)
                    fprintf(output, "%s", rbrack);
            }
            fprintf(output, " %s", key_word);
            
            if (node->type == BIN_OPER)
            {
                if (node->rnode->type != NUM && node->rnode->type != IDENT)
                    fprintf(output, "%s", lbrack);
            }
            generate_node(node->rnode);
            if (node->type == BIN_OPER)
            {
                if (node->rnode->type != NUM && node->rnode->type != IDENT)
                    fprintf(output, "%s", rbrack);
            }
            break;
        case VAR_DECL:
            fprintf(output, "%s %s", key_word, node->value.strval);
            
            if (node->lnode != nullptr)
            {
                PRINT(" became");
                generate_node(node->lnode);
            }
            
            break;
        case FUN_DECL:
            fprintf(output, "%s %s %s", key_word, node->value.strval, lbrack);
            generate_node(node->lnode, 2);
            fprintf(output, "%s\n%s\n", rbrack, lcomp_brack);
            curr_col = 4;
            generate_node(node->rnode, 1);
            fprintf(output, "%s\n", rcomp_brack);
            curr_col = 0;
            
            break;
        case FUN_CALL:
            fprintf(output, " %s%s", node->value.strval, lbrack);
            generate_node(node->lnode, 2);
            fprintf(output, "%s", rbrack);
            break;
        case LINK:
            if (link_type == 1)
            {
                fprintf(output, "%*c", curr_col, ' ');
                generate_node(node->lnode);
                if (node->lnode->type != WHILE_ST && node->lnode->type != COND_ST)
                    fprintf(output, " %s\n", end);
                generate_node(node->rnode, 1);
            }
            else if (link_type == 2)
            {
                generate_node(node->lnode);
                if (node->rnode != nullptr)
                {
                    fprintf(output, " %s", comma);
                    generate_node(node->rnode, 2);
                }
            }
            else if (link_type == 3)
            {
                generate_node(node->lnode);
                generate_node(node->rnode, 3);
            }
            break;
        case INPUT:
            fprintf(output, " %s", key_word);
            break;
        case PRINT:
            fprintf(output, "%s", key_word);
            generate_node(node->lnode);
            break;
        case COND_ST:
            fprintf(output, "%s %s", key_word, lbrack);
            generate_node(node->lnode);
            fprintf(output, "%s\n%*c%s\n", rbrack, curr_col, ' ', lcomp_brack);
            curr_col += 4;
            generate_node(node->rnode->lnode, 1);
            curr_col -= 4;
            fprintf(output, "%*c%s\n", curr_col, ' ', rcomp_brack);
            if (node->rnode->rnode != nullptr)
            {
                fprintf(output, "%*c%s\n%*c%s\n", curr_col, ' ', _else, curr_col, ' ', lcomp_brack);
                curr_col += 4;
                generate_node(node->rnode->rnode, 1);
                curr_col -= 4;
                fprintf(output, "%*c%s\n", curr_col, ' ', rcomp_brack);
            }
            break;
        case WHILE_ST:
            fprintf(output, "%s %s", key_word, lbrack);
            generate_node(node->lnode);
            fprintf(output, "%s\n%*c%s\n", rbrack, curr_col, ' ', lcomp_brack);
            curr_col += 4;
            generate_node(node->rnode, 1);
            curr_col -= 4;
            fprintf(output, "%*c%s\n", curr_col, ' ', rcomp_brack);
            break;
        case RET_ST:
            fprintf(output, "%s", key_word);
            generate_node(node->lnode);
            break;
        default:
            printf("UNKNOWN TYPE : %i\n", node->type);
            break;
        }
    }
    
    #undef PRINT
    
    void destruct()
    {
        if (output != nullptr)
            fclose(output);
    }
    
};
