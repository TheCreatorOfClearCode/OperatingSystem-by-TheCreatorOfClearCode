using System;
using OS.Modules.ModulesControl;
using OS.Modules.SystemModules.Shell;
using System.Collections.Generic;
using System.Globalization;

namespace OS.Modules.StandardModules.Calculator
{
    public class Calculator : IModule, ICommandInfoProvider
    {
        public string Name => "Calculator";
        public string Description => "Evaluates simple math expressions.";
        public string Category => "Utility";

        public void RegisterCommands(CommandDispatcher dispatcher)
        {
            dispatcher.Register("calc", args =>
            {
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Yellow;
                Console.WriteLine("Calculator");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.DarkYellow;
                Console.WriteLine("Enter expressions or type 'exit' to quit, 'clear' to clear the screen.");
                Console.ResetColor();
                Console.ForegroundColor = ConsoleColor.Blue;
                Console.WriteLine("--------------------------------------------------------------------------------");
                Console.ResetColor();

                while (true)
                {
                    Console.Write("-> ");
                    string input = Console.ReadLine()?.Trim();

                    if (string.IsNullOrEmpty(input))
                        continue;

                    if (input.Equals("exit", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.ForegroundColor = ConsoleColor.Blue;
                        Console.WriteLine("--------------------------------------------------------------------------------");
                        Console.ResetColor();
                        Console.ForegroundColor = ConsoleColor.Yellow;
                        Console.WriteLine("Exiting calculator.");
                        Console.ResetColor();
                        Console.ForegroundColor = ConsoleColor.Blue;
                        Console.WriteLine("--------------------------------------------------------------------------------");
                        Console.ResetColor();
                        break;
                    }

                    if (input.Equals("clear", StringComparison.OrdinalIgnoreCase))
                    {
                        Console.Clear();
                        continue;
                    }

                    double result;
                    string error;
                    if (TryEvaluate(input, out result, out error))
                    {
                        Console.ForegroundColor = ConsoleColor.Green;
                        Console.WriteLine("Result: " + result.ToString(CultureInfo.InvariantCulture));
                    }
                    else
                    {
                        Console.ForegroundColor = ConsoleColor.Red;
                        Console.WriteLine("Error: " + error);
                    }
                    Console.ResetColor();
                }
            });
        }

        public Dictionary<string, string> GetCommands()
        {
            return new Dictionary<string, string>
            {
                { "calc", "Evaluates math expressions. Supports +, -, *, /, parentheses, sqrt(x)." }
            };
        }

        private static bool TryEvaluate(string expression, out double result, out string error)
        {
            result = 0;
            error = null;

            try
            {
                var tokens = Tokenize(expression);
                var rpn = ConvertToRPN(tokens);
                return TryEvaluateRPN(rpn, out result, out error);
            }
            catch (Exception ex)
            {
                error = ex.Message;
                return false;
            }
        }

        private static List<string> Tokenize(string expr)
        {
            var tokens = new List<string>();
            int i = 0;
            while (i < expr.Length)
            {
                if (char.IsWhiteSpace(expr[i]))
                {
                    i++;
                    continue;
                }

                if (expr[i] == '-' && (tokens.Count == 0 || tokens[^1] == "(" || "+-*/".Contains(tokens[^1])))
                {
                    string number = "-";
                    i++;
                    while (i < expr.Length && (char.IsDigit(expr[i]) || expr[i] == '.'))
                        number += expr[i++];
                    tokens.Add(number);
                }
                else if (char.IsDigit(expr[i]) || expr[i] == '.')
                {
                    string number = "";
                    while (i < expr.Length && (char.IsDigit(expr[i]) || expr[i] == '.'))
                        number += expr[i++];
                    tokens.Add(number);
                }
                else if (char.IsLetter(expr[i]))
                {
                    string func = "";
                    while (i < expr.Length && char.IsLetter(expr[i]))
                        func += expr[i++];
                    tokens.Add(func);
                }
                else
                {
                    tokens.Add(expr[i].ToString());
                    i++;
                }
            }
            return tokens;
        }

        private static List<string> ConvertToRPN(List<string> tokens)
        {
            var output = new List<string>();
            var stack = new Stack<string>();

            foreach (var token in tokens)
            {
                if (double.TryParse(token, out _))
                {
                    output.Add(token);
                }
                else if (token == "sqrt")
                {
                    stack.Push(token);
                }
                else if (token == "(")
                {
                    stack.Push(token);
                }
                else if (token == ")")
                {
                    while (stack.Count > 0 && stack.Peek() != "(")
                        output.Add(stack.Pop());
                    stack.Pop(); // Remove "("
                    if (stack.Count > 0 && stack.Peek() == "sqrt")
                        output.Add(stack.Pop());
                }
                else if ("+-*/".Contains(token))
                {
                    while (stack.Count > 0 && Precedence(token) <= Precedence(stack.Peek()))
                        output.Add(stack.Pop());
                    stack.Push(token);
                }
            }

            while (stack.Count > 0)
                output.Add(stack.Pop());

            return output;
        }

        private static int Precedence(string op)
        {
            return op switch
            {
                "+" => 1,
                "-" => 1,
                "*" => 2,
                "/" => 2,
                _ => 0,
            };
        }

        private static bool TryEvaluateRPN(List<string> tokens, out double result, out string error)
        {
            var stack = new Stack<double>();
            result = 0;
            error = null;

            foreach (var token in tokens)
            {
                if (double.TryParse(token, out double num))
                {
                    stack.Push(num);
                }
                else if (token == "sqrt")
                {
                    if (stack.Count < 1)
                    {
                        error = "Not enough operands for sqrt";
                        return false;
                    }
                    double x = stack.Pop();
                    if (x < 0)
                    {
                        error = "Cannot take square root of a negative number";
                        return false;
                    }
                    stack.Push(Sqrt(x));
                }
                else if ("+-*/".Contains(token))
                {
                    if (stack.Count < 2)
                    {
                        error = "Not enough operands for operation";
                        return false;
                    }

                    double b = stack.Pop();
                    double a = stack.Pop();

                    if (token == "/" && b == 0)
                    {
                        error = "Division by zero is not allowed";
                        return false;
                    }

                    double res = token switch
                    {
                        "+" => a + b,
                        "-" => a - b,
                        "*" => a * b,
                        "/" => a / b,
                        _ => 0
                    };

                    stack.Push(res);
                }
            }

            if (stack.Count == 1)
            {
                result = stack.Pop();
                return true;
            }
            else
            {
                error = "Invalid expression";
                return false;
            }
        }

        private static double Sqrt(double x)
        {
            if (x == 0) return 0;
            double guess = x / 2.0;
            for (int i = 0; i < 10; i++)
            {
                guess = (guess + x / guess) / 2.0;
            }
            return guess;
        }
    }
}