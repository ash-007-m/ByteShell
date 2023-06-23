#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sstream>
#include <list> 

using namespace std;

int input_size;
int (*bulldins[8])(char **tokens);
char **bulldins_func = new char *[8];
const int f_size = 7;
unordered_map<string, char **> alias_data;
unordered_map<string, int> v_let;
list<string> data_his;
bool convert_to_int(string s, int &ans)
{

    for (int i = s.size() - 1; i >= 0; i--)
    {
        if (s[i] > '9' || s[i] < '0')
            return 0;
    }
    ans = stoi(s);
    return 1;
}
int history(char **tokens) // input is to just sync it with other functions
{
    auto it = data_his.begin();
    while (it != data_his.end())
    {
        cout << *it << endl;
        it++;
    }
    data_his.push_back("history called");
    if (data_his.size() > 15)
        data_his.pop_front();
    return 1;
}
int help(char **tokens)
{
    string temp = "help called for ";
    if (tokens[2] != nullptr)
        temp = temp + string(tokens[2]) + "...";
    data_his.push_back(temp);
    if (data_his.size() > 15)
        data_his.pop_front();

    bool i_h = 0;
    if (tokens[2] != nullptr)
    {
        cout << "Invalid argument " << endl;
        return -1;
    }
    if (string(tokens[1]) == "create_alias")
    {
        cout << " Syntax create_alias name command " << endl;
        cout << " creat_alias command created an short form for your command which you can futher use" << endl;
        i_h = 1;
    }
    if (string(tokens[1]) == "un_alias")
    {
        cout << " Syntax un_alias name1 name2 ..." << endl;
        cout << " un_alias command removes existing alias " << endl;
        cout<<  " un_alias -a , this command removes all existing alias"<<endl;
        i_h = 1;
    }
    if (string(tokens[1]) == "let")
    {
        cout << "Syntax let varname [ expression ] var2 [ expression ] var3 value++ ..." << endl;
        cout << "It evulates the value corresponding to next epression " << endl;
        cout << "Incrrement , dcrement operator (++), (--) and (+var), (-var) are also supprted" << endl;
        cout << "ex --> let v1 v2++  this assigns v1 value of v2 and then increment v2 rest are similar" << endl;
        i_h = 1;
    }
    if (string(tokens[1]) == "history")
    {
        cout << " Syntax history " << endl;
        cout << " Shows recent 15 commands " << endl;
        i_h = 1;
    }

    if (string(tokens[1]) == "echo")
    {
        cout << " Syntax echo $va1 va2 ..." << endl;
        cout << " If $ is used before variable it shows value other output variable name as stirng " << endl;
        i_h = 1;
    }
    if (string(tokens[1]) == "bulldin")
    {
        cout << " Syntax bulldin function_name argument_for_function" << endl;
        cout << " Can be used to call any inbuild function directly in our case create_alias, un_alias, echo, let .." << endl;
        i_h = 1;
    }
    if (i_h)
        return 1;

    cout << "Invalid command!" << endl;
    return -1;
}
int let(char **tokens)
{
   
    string temp="let ";
    if(tokens[1]!=nullptr )temp=temp+string(tokens[1])+"...";
     data_his.push_back(temp);
    if (data_his.size() > 15)
        data_his.pop_front();
    char pp[] = "++";
    char mm[] = "--";
    char p[] = "+";
    char m[] = "-";

    // input is of the form let var1 expression va2 expression ...
    for (int i = 1; tokens[i] != nullptr and tokens[i + 1] != nullptr; i = i)
    {
        int t_v = 0;
        // =++var,=--var,=+var,=-var
        bool d = 0;
        string in = string(tokens[i + 1]);
        string t_s = string(tokens[i]);
        char su = '[';
        if (in[0] == '[') // its an expression
        {
            i = i + 2;
            if (tokens[i] == nullptr)
            {
                cout << "invalid argument" << endl;
                return -1;
            }

            int value = 0;
            char cc[] = "]";
            bool c = 0;
            bool i_v = 1;
            char pre = '+';
            while (tokens[i] != nullptr && strncmp(cc, tokens[i], 1) != 0)
            {
                int value_of_sting;
                if (c)
                {
                    if (v_let.count(tokens[i]) || convert_to_int(string(tokens[i]), value_of_sting))
                    {
                        cout << "invalid argument" << endl;
                        return -1;
                    }
                    c = 0;
                    i_v = 1;
                    if (string(tokens[i]) == "+")
                        pre = '+';
                    if (string(tokens[i]) == "-")
                        pre = '-';
                    if (string(tokens[i]) == "*")
                        pre = '*';
                    if (string(tokens[i]) == "/")
                        pre = '/';
                    i++;
                    continue;
                }
                if (i_v)
                {
                    if (v_let.count(tokens[i]) == 0 and !convert_to_int(string(tokens[i]), value_of_sting))
                    {
                        cout << "invalid argument" << endl;
                        return -1;
                    }
                    else
                    {
                        if (v_let.count(tokens[i]))
                        {
                            if (pre == '+')
                                value += v_let[tokens[i]];
                            if (pre == '-')
                                value -= v_let[tokens[i]];
                            if (pre == '*')
                                value *= v_let[tokens[i]];
                            if (pre == '/')
                            {
                                if (v_let[tokens[i]] == 0)
                                {
                                    cout << "division by 0" << endl;
                                    return -1;
                                }
                                value /= v_let[tokens[i]];
                            }
                        }
                        else
                        {
                            if (pre == '+')
                                value += value_of_sting;
                            if (pre == '-')
                                value -= value_of_sting;
                            if (pre == '*')
                                value *= value_of_sting;
                            if (pre == '/')
                            {
                                if (value_of_sting == 0)
                                {
                                    cout << "division by 0" << endl;
                                    return -1;
                                }
                                value /= value_of_sting;
                            }
                        }
                    }
                    c = 1;
                    i_v = 0;
                }
                i++;
            }
            if (tokens[i] == nullptr)
            {
                cout << "invalid argument" << endl;
                return -1;
            }
            v_let[t_s] = value;
            i++;
            continue;
        }
        if (strncmp(pp, tokens[i + 1], 2) == 0)
        {

            string var = in.substr(2, in.length() - 2);
            v_let[tokens[i]] = ++v_let[var];
            d = 1;
        }
        if (strncmp(mm, tokens[i + 1], 2) == 0)
        {

            string var = in.substr(2, in.length() - 2);
            v_let[tokens[i]] = --v_let[var];
            d = 1;
        }
        if (strncmp(p, tokens[i + 1], 1) == 0)
        {

            string var = in.substr(1, in.length() - 1);
            v_let[tokens[i]] += v_let[var];
            d = 1;
        }
        if (strncmp(m, tokens[i + 1], 1) == 0)
        {

            string var = in.substr(1, in.length() - 1);
            v_let[tokens[i]] -= v_let[var];
            d = 1;
        }
        if (d == 0)
        {
            if (in.length() >= 3 && in[in.length() - 1] == '+' && in[in.length() - 2] == '+')
            {
                string var = in.substr(0, in.length() - 2);
                v_let[tokens[i]] = v_let[var]++;
                d = 1;
            }
            if (in.size() >= 3 && in[in.length() - 1] == '-' && in[in.length() - 2] == '-')
            {
                string var = in.substr(0, in.length() - 2);
                v_let[tokens[i]] = v_let[var]--;
                d = 1;
            }
        }
        if (d == 0) // standard num value or var
        {
            bool is_num = 1;
            for (int j = 0; j < in.length(); j++)
            {
                if (in[j] > '9' || in[j] < '0')
                {
                    is_num = 0;
                    break;
                }
            }
            if (is_num)
            {
                v_let[tokens[i]] = stoi(in);
                d = 1;
            }
        }
        if (d == 0)
        {
            if (v_let.count(in) > 0)
            {
                v_let[tokens[i]] = v_let[in];
            }
            else
            {
                cout << "invalid argument" << endl;
                return -1;
            }
        }
        i = i + 2;
    }
    cout << "expresion evluated" << endl;
    return 1;
}
int echo(char **tokens)
{
     string temp="echo ";
    if(tokens[1]!=nullptr )temp=temp+string(tokens[1])+"...";
     data_his.push_back(temp);
    if (data_his.size() > 15)
        data_his.pop_front();
    char c[] = "$";
    for (int i = 1; tokens[i] != nullptr; i++)
    {
        string temp = string(tokens[i]);
        if (strncmp(c, tokens[i], 1) == 0)
        {
            temp = temp.substr(1, temp.length() - 1);
            if (v_let.count(temp) == 0)
            {
                cout << "invalid argument" << endl;
                return -1;
            }
            else
            {
                cout << v_let[temp] << " ";
            }
        }
        else
        {
            cout << tokens[i] << " ";
        }
    }
    cout << endl;
    return 1;
}
int bulldin(char **tokens)
{
     string temp="bulldin ";
    if(tokens[1]!=nullptr )temp=temp+string(tokens[1])+"...";
     data_his.push_back(temp);
    if (data_his.size() > 15)
        data_his.pop_front();
    char *final_input[100]; // max input size;
    int j = 0;
   // cout << "bulldin called " << endl;
    for (int i = 1; tokens[i] != nullptr; i++)
    {
        final_input[j++] = tokens[i];
    }

    final_input[j] = nullptr;
    input_size = j;
    for (int i = 0; i < f_size; i++)
    {
        if (strcmp(bulldins_func[i], tokens[1]) == 0)
            return (*bulldins[i])(final_input);
    }
    return -1; // no bulldins exist
}
int create_alias(char **tokens)
{
     string temp="create_alias ";
    if(tokens[1]!=nullptr )temp=temp+string(tokens[1])+"...";
     data_his.push_back(temp);
    if (data_his.size() > 15)
        data_his.pop_front();
   // cout << "called alias" << endl;
    if (tokens[1]==nullptr || alias_data.count(tokens[1]) || tokens[2]==nullptr)
        {   cout<<"invalied input !"<<endl;
            return -1;
        }
    for (int i = 0; i < f_size; i++)
    {
        if (strcmp(tokens[1], bulldins_func[i]) == 0)
        {
            return -1; // Alias name is already a built-in function
        }
    }

    char **alias_commands = new char *[input_size - 2];

    for (int i = 2; i < input_size; i++)
    {
        alias_commands[i - 2] = new char[strlen(tokens[i]) + 1];
        strcpy(alias_commands[i - 2], tokens[i]);
    }

    alias_data[tokens[1]] = alias_commands;
    cout << "alias created" << endl;
    return 0; // Success
}
int un_alias(char **tokens)
{
     string temp="un_alias ";
    if(tokens[1]!=nullptr )temp=temp+string(tokens[1])+"...";
     data_his.push_back(temp);
    if (data_his.size() > 15)
        data_his.pop_front();
    bool a = 0;
    char check[] = "-a";
    if (strcmp(check, tokens[1]) == 0)
    {
        alias_data.clear();
        return 1;
    }
    for (int i = 1; tokens[i] != nullptr; i++)
    {
        if (alias_data.count(tokens[i]))
        { // cout<<"deleting "<<tokens[i]<<endl;
            alias_data.erase(tokens[i]);
            a = 1;
        }
    }

    return a ? a : -1;
}

int Launch(char **tokens)
{
    string temp = "launch called for ";
    temp += string(tokens[0]);
    if (tokens[1] != nullptr)

        temp += string(tokens[1]);
    temp += "...";
    data_his.push_back(temp);
    if (data_his.size() > 15)

        data_his.pop_front();

    pid_t pid = fork();
    int status;

    if (pid == 0)
    { // Child process
        int result = execvp(tokens[0], tokens);
        if (result == -1)
            perror("ByteShell>>");
        exit(1);
    }
    else if (pid > 0)
    { // Parent process
        do
        {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    else
    { // Forking failed
        perror("Fork failed");
        return -1;
    }

    return 1;
}

int Execute(char **tokens)
{
    for (int i = 0; i < f_size; i++)
    {
        if (strcmp(tokens[0], bulldins_func[i]) == 0)
        {

            return (*bulldins[i])(tokens); // If any built-in matches with input
        }
    }

    //cout << "Calling launch" << endl;
    return Launch(tokens); // None of the built-ins matches, launch the command
}

int ByteShell()
{
    while (true)
    {
        string input;
        cout << "ByteShell>> ";
        getline(cin, input);

        stringstream check1(input);
        string temp;
        vector<string> tokens;

        while (getline(check1, temp, ' '))
        {
            tokens.push_back(temp);
        }

        if (tokens[0] == "terminate")
        {

            break;
        }

        input_size = tokens.size();
        char *toin[10000]; // Max limit of a line 10000 words
        int j = 0;
        char check[] = "un_alias";
        for (int i = 0; i < tokens.size(); i++)
        {
            if (alias_data.count(tokens[i]) and (j == 0 || strcmp(toin[j - 1], check) != 0))
            { // We have an alias, replace commands

                char **alias_commands = alias_data[tokens[i]];
                int k = 0;
                while (alias_commands[k] != nullptr)
                {
                    toin[j++] = alias_commands[k++];
                }
            }
            else
            {
                toin[j] = new char[tokens[i].size() + 1];
                strcpy(toin[j], tokens[i].c_str());
                j++;
            }
        }

        toin[j] = nullptr; // Null-terminate the array
        // for (int i = 0; i < j; i++)
        //     cout << toin[i] << " ";
        // cout << endl;
        int result = Execute(toin);
        // if (result == 1)
        //     cout << "opreation succesful" << endl;
    }

    return 1;
}
void build()
{
    bulldins[0] = create_alias;
    string temp = "create_alias";
    bulldins_func[0] = new char[temp.size() + 1];
    strcpy(bulldins_func[0], temp.c_str());
    bulldins[1] = un_alias;
    temp = "un_alias";
    bulldins_func[1] = new char[temp.size() + 1];
    strcpy(bulldins_func[1], temp.c_str());
    bulldins[2] = bulldin;
    temp = "bulldin";
    bulldins_func[2] = new char[temp.size() + 1];
    strcpy(bulldins_func[2], temp.c_str());
    bulldins[3] = let;
    temp = "let";
    bulldins_func[3] = new char[temp.size() + 1];
    strcpy(bulldins_func[3], temp.c_str());
    bulldins[4] = echo;
    temp = "echo";
    bulldins_func[4] = new char[temp.size() + 1];
    strcpy(bulldins_func[4], temp.c_str());
    bulldins[5] = history;
    temp = "history";
    bulldins_func[5] = new char[temp.size() + 1];
    strcpy(bulldins_func[5], temp.c_str());
    bulldins[6] = help;
    temp = "help";
    bulldins_func[6] = new char[temp.size() + 1];
    strcpy(bulldins_func[6], temp.c_str());
}

int main()
{

    build();
    ByteShell();

    return 0;
}
