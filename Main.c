#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char keyWords[][10] = {"int", "text", "is", "loop", "times", "read", "write", "newLine"};
char operators[][1] = {"+", "-", "*", "/"};
char specialChars[] = {'+', '-', '*', '/', '.', '\n', ',', '{', '}'};

FILE *createFile(char *filePath);

char intIdentifiers[10][100];
int lastInt;    // last empty integer identifier index.
int intVar[10]; // integer values.

char strIdentifiers[10][270];
int lastStr;            // last empty text identifier index.
char strNames[10][100]; // text names.

int leftBraces[20]; // for right left curly operations.
int leftIndex = 0;

int rightBraces[20]; // for right left curly operations.
int rightIndex = 0;

char lexLines[150][300]; // holds lexical analyzer outputs.
int currentIndex;        // holds the lex lines index where interpreter operates.

void strControl();
void readControl();
void intControl();
void writeControl();
void loopControl();

int strToInt(char *intStr) {
    int number = atoi(intStr);
    return number;
}

int isSpecial(char indexCharacter) {
    int index = 0;
    while (specialChars[index] != '\0') {
        if (indexCharacter == specialChars[index])
            return 1;
        index++;
    }
    return 0;
}

void reset(char *str) {
    int uzunluk = strlen(str);
    for (int i = 0; i < uzunluk; i++) {
        str[i] = '\0';
    }
}

int isInKeywords(char *str) {
    for (int i = 0; i < 8; i++) {
        if (strcmp(str, keyWords[i]) == 0) {
            return 1; // Keyword found.
        }
    }
    return 0; // Keyword not found.
}

int isInOperators(char str) {
    for (int i = 0; i < 4; i++) {
        if (str == operators[0][i]) {
            return 1;
        }
    }
    return 0;
}

int identifierControl(char *str, FILE *ptr) {
    if (strlen(str) > 10) {
        fprintf(ptr, "Error: Identifier is too long.\n");
        return 0;
    } else if (!isalpha(str[0])) {
        fprintf(ptr, "Error: Identifier must start with a letter.\n");
        return 0;
    } else {
        fprintf(ptr, "Identifier(%s)\n", str);
        return 0;
    }
}

int integerControl(int num, FILE *ptr) {
    if (num < 0) {
        integerControl(0, ptr); // Recursive call to negative nums.
    } else {
        fprintf(ptr, "IntConts(%d)\n", num);
        return 1;
    }
}

int operatorControl(char *str, FILE *ptr) {
    if (strlen(str) != 1) {
        return 0;
    } else if (isInOperators(str[0])) {
        fprintf(ptr, "Operator(%s)\n", str);
        return 1;
    } else {
        return 0;
    }
}

int bracketControl(char *str, FILE *ptr) {
    if (str[0] == '{') {
        fprintf(ptr, "LeftCurlyBracket\n");
        return 1;
    } else if (str[0] == '}') {
        fprintf(ptr, "RightCurlyBracket\n");
        return 1;
    } else {
        return 0;
    }
}
int stringControl(char *str, FILE *ptr) {
    if (strlen(str) > 256) {
        fprintf(ptr, "String(\"%s\")\n", str);
        return 0;
    } else {
        fprintf(ptr, "String(\"%s\")\n", str);
        return 1;
    }
}

int keywordControl(char *str, FILE *ptr) {
    if (isInKeywords(str)) {
        fprintf(ptr, "Keyword(%s)\n", str);
        return 1;
    } else {
        return 0;
    }
}

int endOfLineControl(char *str, FILE *ptr) {

    if (str[0] == '.') {
        fprintf(ptr, "EndOfLine\n");
        return 1;
    } else {
        return 0;
    }
}

int commaControl(char str[100], FILE *ptr) {
    if (str[0] == ',') {
        fprintf(ptr, "Comma\n");
        return 1;
    } else {
        return 0;
    }
}

void fileReading(char *filePath) {
    FILE *fptr;
    fptr = fopen(filePath, "r");
    FILE *newPtr;
    newPtr = createFile(filePath);

    char lineString[100];
    char tokenString[257] = {'\0'}; // for tokenStrings to identify which token it is.
    int commentControl = 0;
    int strconControl = 0;
    int tokenIndex = 0;
    // Read the line content and print it
    while (fgets(lineString, 100, fptr)) { // Beginning of everyline of the file.
        int index = 0;
        // indexes of the string which will be parameterized.
        int startIndex = 0;
        while (lineString[index] != '\0') { // Beginning of everyindex of the line.

            char charOfIndex = lineString[index];
            char charOfStart = lineString[startIndex];

            char commentStrings[] = {charOfIndex, lineString[index + 1], '\0'};
            int commentCondition = strcmp("/*", commentStrings) == 0 && tokenString[0] == '\0';
            // Comment finding
            if (commentCondition && !commentControl) { // If they are the same:

                index = index + 2;
                startIndex = index;
                commentControl = 1;

                continue;
            } else if (commentControl && tokenString[0] == '\0') { // If commentControl is 1, it means searching for "*/".
                if (strcmp("*/", commentStrings) == 0) {
                    index = index + 2;
                    startIndex = index;
                    commentControl = 0;
                } else {
                    index++;
                }
                continue;
            }
            // String tokenizer.
            if (charOfIndex == '"' && strconControl == 0 && tokenString[0] == '\0') { // If strconControl is 1, it means searching for "*/".
                index++;
                startIndex = index;
                strconControl = 1;
                continue;
            } else if (strconControl == 1 && charOfIndex == '"') {
                stringControl(tokenString, newPtr);
                strconControl = 0;
                index++;
                startIndex++;
                reset(tokenString);
                tokenIndex = 0;
                continue;
            } else if (strconControl) {
                if (charOfIndex != '\n') {
                    tokenString[tokenIndex++] = charOfIndex;
                    tokenString[tokenIndex] = '\0';
                }
                index++;
                startIndex++;
                continue;
            }

            // Integer tokenizer.
            // Finds integer and tokenizes it.
            int firstCond = isdigit(charOfStart);
            int secondCond = (charOfStart == '-' && isdigit(lineString[startIndex + 1]));
            int condition = isdigit(charOfIndex) || (charOfIndex == '-' && index == startIndex);

            if (firstCond || secondCond) {
                if (condition) {
                    tokenString[tokenIndex++] = lineString[index++];
                    continue;
                }
                // If enters else block, it means end of the integer token and it is time to tokenize the value.
                else {
                    startIndex = index;
                    tokenString[tokenIndex] = '\0';

                    int returnedInt = strToInt(tokenString);
                    integerControl(returnedInt, newPtr);
                    reset(tokenString);
                    tokenIndex = 0;

                    continue;
                }
                // nonInteger tokenizer.
                // Finds nonIntegers and tokenizes it.
            } else {
                int condition = charOfIndex != ' ' && charOfIndex != '\n';
                int condition2 = !isSpecial(charOfIndex) && charOfIndex != '"';

                if (condition && condition2) {
                    tokenString[tokenIndex++] = lineString[index++];
                    continue;
                }
                // If enters else block, it means end of the nonInteger token and it is time to tokenize the value.
                else {
                    startIndex = index;
                    tokenString[tokenIndex] = '\0';

                    if (keywordControl(tokenString, newPtr)) {
                    } else if (isalpha(charOfStart)) {
                        identifierControl(tokenString, newPtr);
                    }
                    if (charOfIndex == '/' && lineString[index + 1] == '*') {
                        if (commentControl) {
                            continue;
                        } else {
                            reset(tokenString);
                            tokenIndex = 0;

                            continue;
                        }
                    }
                    if (isSpecial(charOfIndex)) {
                        tokenString[0] = charOfIndex;
                        tokenString[1] = '\0';
                    }

                    if (isInOperators(charOfIndex)) {
                        operatorControl(tokenString, newPtr);
                    }

                    if (charOfIndex == '{' || charOfIndex == '}') {
                        bracketControl(tokenString, newPtr);
                    }
                    if (charOfIndex == '.') {
                        endOfLineControl(tokenString, newPtr);
                    }
                    if (charOfIndex = ',') {
                        commaControl(tokenString, newPtr);
                    }

                    reset(tokenString);
                    tokenIndex = 0;

                    if (lineString[index] == '"')
                        continue;
                }
            }

            startIndex++;
            index++;
            // Every end of the index, we check the next index.
        }
    }
    if (commentControl) {
        fprintf(newPtr, "Error! Comment part is not completed!");
    }
    if (strconControl) {
        fprintf(newPtr, "Error! String constant part is not completed!");
    }

    fclose(fptr);
    fclose(newPtr);
}

FILE *createFile(char *filePath) {
    // Creates a file with correct extension.
    // Takes the file name and combine it with file extension ".lex".
    int len = strlen(filePath);
    char newPath[len + 1];
    int i = 0;
    if (!strstr(filePath, ".sta")) {
        printf("Only acceptable for .sta file extension type!");
        return NULL;
    }
    while (filePath[i] != '.') {
        newPath[i] = filePath[i];
        i++;
    }
    newPath[i] = '\0';
    char sendPath[len + 1];
    sprintf(sendPath, "%s.lex", newPath);
    FILE *ptr;
    ptr = fopen(sendPath, "w");

    return ptr;
}

// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------I N T E R P R E T E R------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------------------

void lexLinesCreate(char *filePath) { // takes the strings in aa.lex and writes every elements into lexLines strings.
    FILE *fptr;
    fptr = fopen(filePath, "r");
    char lineString[300];
    int i = 0;
    while (fgets(lineString, 300, fptr)) {
        strcpy(lexLines[i], lineString);
        i++;
    }
}

int isSameInt(char *str) { // checks if the int value in the parameter is the same as in intIdentifiers array.
    int i = 0;
    while (intIdentifiers[i][0] != '\0') {
        if (strcmp(str, intIdentifiers[i]) == 0) {
            return 1;
        }
        i++;
    }
    return 0;
}

int isSameTxt(char *str) { // checks if the string value in the parameter is the same as in strIdentifiers array.
    int i = 0;
    while (strIdentifiers[i][0] != '\0') {
        if (strcmp(str, strIdentifiers[i]) == 0) {
            return 1;
        }
        i++;
    }
    return 0;
}

void toIdent(char *str, char *copied) { // takes the Identifier token from lexical file and takes just identifier name in that token.
    if (strstr(copied, "Identifier")) {
        int i = 11;
        int s = 0;
        while (copied[i] != ')') {
            str[s] = copied[i];
            i++;
            s++;
        }
        str[s] = '\0';
    }
}

int endIndex() { // returns index involving of EndOfLine.
    int temp = currentIndex;
    while (lexLines[temp][0] != '\0') {

        if (strcmp(lexLines[temp], "EndOfLine\n") == 0) { // lexLines[temp] == "EndOfLine"

            break;
        }
        temp++;
    }

    return temp;
}

int checkIntIndex(char *str) { // controls if given parameter has already exists in int Identifiers.
    if (strstr(str, "Identifier") == 0) {
        return -1;
    }
    char ident[100];
    toIdent(ident, str);
    int i = 0;
    while (i < 10) {
        if (strcmp(intIdentifiers[i], ident) == 0) { // intIdentifiers[i] == str
            return i;
        }
        i++;
    }
    return -1;
}

int checkStrIndex(char *str) { // controls if given parameter has already exists in string Identifiers.
    if (strstr(str, "Identifier") == 0) {
        return -1;
    }
    char ident[100];
    toIdent(ident, str);
    int i = 0;
    while (i < 10) {
        if (strcmp(strIdentifiers[i], ident) == 0) { // strIdentifiers[i] == str
            return i;
        }
        i++;
    }
    return -1;
}

int checkLineIndex(char *str) { // returns end of the statement index.
    int i = currentIndex;
    while (!strstr(lexLines[i], "EndOfLine") && lexLines[i][0] != '\0') {
        if (strcmp(lexLines[i], str) == 0) { // lexLines[i] == str
            return i;
        }
        i++;
    }
    return -1;
}

int toInt(char *str) { // takes the IntConst token and converts it into int value.
    int start = 0;
    int i = 9;
    char number[100];
    while (str[i] != ')') {
        number[start] = str[i];
        i++;
        start++;
    }
    number[start] = '\0';
    int returnNum = atoi(number);
    return returnNum;
}

void toStr(char *modify, char *arranged) { // takes the String token and converts it into string value.
    int start = 0;
    int i = 8;

    char *string = (char *)malloc(sizeof(char) * 260);
    while (arranged[i] != '"') {
        if (start > 256) {
            start++;
            break;
        } else {
            string[start] = arranged[i];
            start++;
            i++;
        }
    }
    string[start] = '\0';
    strcpy(modify, string);
    free(string);
}
void substractTxt(char operand1[], char *subtracted) { // subtracts subtracted from operand1.
    char *result = strstr(operand1, subtracted);
    int subtractedSize = strlen(subtracted);
    if (result) {
        char *nextSubtracted = result + subtractedSize;
        int subIndex = nextSubtracted - operand1;
        int length = strlen(operand1) + 1 - (subIndex);
        memmove(result, nextSubtracted, length);
    }
}

void txtOperator(char lexLines[][300], int temp) { // operands text values.
    char operand1[270], operand2[270];

     if (strstr(lexLines[temp + 1], "Identif")) {
        int index1 = checkStrIndex(lexLines[temp + 1]);
        strcpy(operand1, strNames[index1]);
    } 
    else if (strstr(lexLines[temp + 1], "String")) {
        toStr(operand1, lexLines[temp + 1]);
    }

     if (strstr(lexLines[temp + 3], "Identif")) {
        int index2 = checkStrIndex(lexLines[temp + 3]);
        strcpy(operand2, strNames[index2]);
    } 
    else if (strstr(lexLines[temp + 3], "String")) {
        toStr(operand2, lexLines[temp + 3]);
    } 

    char operator= lexLines[temp + 2][9];
    int control = operator== '+' || operator== '-';

    switch (operator) {
    case '+':
        strcat(operand1, operand2);
        int i = checkStrIndex(lexLines[temp - 1]);
        strcpy(strNames[i], operand1);
        break;
    case '-':
        substractTxt(operand1, operand2);
        int y = checkStrIndex(lexLines[temp - 1]);
        strcpy(strNames[y], operand1);
        break;
    }
}

int intOperator(char lexLines[][300], int temp) { // operands int values.
    int operand1, operand2;

    if (strstr(lexLines[temp + 1], "IntConts")) {
        operand1 = toInt(lexLines[temp + 1]);
    } else if (strstr(lexLines[temp + 1], "Identif")) {
        int index1 = checkIntIndex(lexLines[temp + 1]);
        operand1 = intVar[index1];
    }

    if (strstr(lexLines[temp + 3], "IntConts")) {
        operand2 = toInt(lexLines[temp + 3]);
    } else if (strstr(lexLines[temp + 3], "Identif")) {
        int index2 = checkIntIndex(lexLines[temp + 3]);
        operand2 = intVar[index2];
    }

    char operator= lexLines[temp + 2][9];
    int control = operator== '+' || operator== '-' || operator== '/' || operator== '*';
    int rtn;
    if (control) {

        switch (operator) {
        case '+':
            rtn = operand1 + operand2;
            break;
        case '-':
            rtn = operand1 - operand2;
            break;
        case '*':
            rtn = operand1 * operand2;
            break;
        case '/':
            rtn = operand1 / operand2;
            break;
        }
        if (rtn >= 0 && rtn <= 99999999) {
            return rtn;
        } else if (rtn >= 99999999) {
            printf("Max Integer value is 99999999! ");
            return 0;
        } else
            return 0;
    }

    return -1;
}
int txtOrInt(char *lexLine) { // returns 1 for  text, 2 for int about variable type.
    int control = checkStrIndex(lexLine);
    if (control != -1)
        return 1;

    control = checkIntIndex(lexLine);
    if (control != -1)
        return 2;
}

void oneRev() { // processes every statement of star code.
    int temp = currentIndex;
    int endInd = endIndex();

    int control = currentIndex == temp; // if statement is processed, control is false.

    if (control)
        loopControl();
    control = currentIndex == temp;

    if (control)
        strControl();
    control = currentIndex == temp;

    if (control)
        intControl();
    control = currentIndex == temp;

    if (control)
        readControl();
    control = currentIndex == temp;

    if (control)
        writeControl();
    control = currentIndex == temp;
}

int numberEnd(int endCounterIndex) { // finds statement number in loops having curly brackets.
    int endControl = endCounterIndex;
    int i = 0;
    while (!strstr(lexLines[endControl], "RightCurlyBracket") || leftBraces[0] != 0) {
        char *lexLine = lexLines[endControl];
        if (strstr(lexLine, "LeftCurlyBracket")) {
            leftBraces[leftIndex++] = 1;
        } else if (strstr(lexLine, "RightCurlyBracket")) {
            leftBraces[--leftIndex] = 0;
            i++;
        } else if (strstr(lexLine, "EndOfLine")) {
            i++;
        }
        endControl++;
    }
    return i;
}
int findRightCurly(int endCounterIndex) { // finds rightcurlybracket index of leftcurlybracket in lexLines.
    while (!strstr(lexLines[endCounterIndex], "RightCurlyBracket") || rightBraces[0] != 0) {
        char *lexLine = lexLines[endCounterIndex];
        if (strstr(lexLine, "LeftCurlyBracket")) {
            rightBraces[rightIndex++] = 1;
        } else if (strstr(lexLine, "RightCurlyBracket")) {
            rightBraces[--rightIndex] = 0;
        }

        endCounterIndex++;
    }
    return endCounterIndex;
}

void curlyLoop() { // operands loop with curly brackets.
    int loopValue;
    char *result1 = strstr(lexLines[currentIndex + 1], "IntConts");

    if (result1) {
        loopValue = toInt(lexLines[currentIndex + 1]);
    }
    char *result2 = strstr(lexLines[currentIndex + 3], "LeftCurlyBracket");

    int endCounterIndex = currentIndex + 4;
    int rightCurlyBracket = findRightCurly(endCounterIndex);

    int endNumber = numberEnd(endCounterIndex);

    if (result2) {
        currentIndex = endCounterIndex;
        int j = 0;

        while (j < loopValue) {
            int i = 0;
            while (i < endNumber) {
                oneRev();
                i++;
            }
            currentIndex = endCounterIndex;
            j++;
        }
        currentIndex = rightCurlyBracket + 1;
    }
}

void normalLoop() { // operands loops without curly brackets.
    int loopValue;
    char *result1 = strstr(lexLines[currentIndex + 1], "IntConts");

    if (result1) {
        loopValue = toInt(lexLines[currentIndex + 1]);
    }
    char *result2 = strstr(lexLines[currentIndex + 2], "Keyword(times)");
    int isLoop = result1 && result2;

    if (isLoop) {
        int i = 0;
        currentIndex = currentIndex + 3;
        int temp = currentIndex;

        while (i < loopValue) {
            oneRev();
            currentIndex = temp;
            i++;
        }
        int endInd = endIndex();
        currentIndex = endInd + 1;
    }
}

void loopControl() { // operands loops.
    int endInd = endIndex();
    int temp = currentIndex;
    if (lexLines[temp][0] == '\0') {
        exit(0);
    }

    if (strstr(lexLines[currentIndex], "Keyword(loop)")) {
        char *result2 = strstr(lexLines[currentIndex + 3], "LeftCurlyBracket");
        if (result2) {
            curlyLoop();
        } else
            normalLoop();
    }
}

void ifNonPrompt(int temp) { // operand statements with "read" keywords without strings.
                             // read  varName.
    int control = txtOrInt(lexLines[temp + 1]);
    int index;
    int val;
    char strval[270];
    if (control == 1) {
        scanf("%s", &strval);
        index = checkStrIndex(lexLines[temp + 1]);
        strcpy(strNames[index], strval);
    } else if (control == 2) {
        index = checkIntIndex(lexLines[temp + 1]);
        if (scanf("%d", &val)) {
            if (val < 0) {
                val = 0;
            } else if (val > 99999999) {
                printf("Int value cannot be larger than 99999999!");
                val = 0;
            }
            intVar[index] = val;
        } else {
            index = checkIntIndex(lexLines[temp + 1]);
            intVar[index] = 0;
        }
    }
}

void ifString(int temp) { // operand statements with "read" keywords with strings.
                          // read "prompt:", varName.
    if (strstr(lexLines[temp + 2], "Comma")) {
        char prompt[270];
        int isIdent = checkStrIndex(lexLines[temp + 1]);

        if (strstr(lexLines[temp + 1], "String")) {
            toStr(prompt, lexLines[temp + 1]);
        } else if (isIdent != -1) {
            strcpy(prompt, strNames[isIdent]);
        }

        int control = txtOrInt(lexLines[temp + 3]);
        if (control == 2) {
            int val;
            int intIndex = checkIntIndex(lexLines[temp + 3]);
            printf("%s", prompt);
            if (scanf("%d", &val)) {
                if (val < 0) {
                    val = 0;
                } else if (val > 99999999) {
                    printf("Int value cannot be larger than 99999999!");
                    val = 0;
                }
                intVar[intIndex] = val;
            } else {
                printf("You did not enter a numeric value!");
                intVar[intIndex] = 0;
            }
        }
        if (control == 1) {
            char str[270];
            int txtIndex = checkStrIndex(lexLines[temp + 3]);
            printf("%s", prompt);
            scanf("%s", str);
            strcpy(strNames[txtIndex], str);
        }
    }
}

void readControl() { // operands read statements.
    int endInd = endIndex();
    int temp = currentIndex;
    if (lexLines[temp][0] == '\0') {
        exit(0);
    }
    if (strstr(lexLines[currentIndex], "Keyword(read)")) {
        if (strstr(lexLines[temp + 2], "EndOfLine")) {
            ifNonPrompt(temp);
        } else if (strstr(lexLines[temp + 4], "EndOfLine")) {
            ifString(temp);
        }

        currentIndex = endInd + 1;
    }
}

void writeControl() { // operands write statements.
    int endInd = endIndex();
    int temp = currentIndex;
    if (lexLines[temp][0] == '\0') {
        exit(0);
    }
    if (strstr(lexLines[currentIndex], "Keyword(write)")) {

        while (strcmp(lexLines[temp], "EndOfLine\n")) {
            if (strstr(lexLines[temp], "Identifier")) {
                int checkStr = checkStrIndex(lexLines[temp]);
                char *writeStr = strNames[checkStr];
                if (checkStr != -1) {
                    printf("%s", writeStr);
                }
                int checkInt = checkIntIndex(lexLines[temp]);
                if (checkInt != -1) {
                    int intVal = intVar[checkInt];
                    printf("%d", intVal);
                }

            }
            else if (strstr(lexLines[temp], "String")) {
                char *wrtStr = (char *)malloc(sizeof(char) * 300);
                toStr(wrtStr, lexLines[temp]);

                printf("%s", wrtStr);

            } else if (strstr(lexLines[temp], "IntConts")) {
                int val = toInt(lexLines[temp]);
                printf("%d", val);
            } 

            else if (strcmp(lexLines[temp], "Comma\n") == 0) { // lexLines[temp] == "Comma"
                temp++;
                continue;
            }

            temp++;
        }
        currentIndex = endInd + 1;
    } else if (strstr(lexLines[currentIndex], "newLine")) {
        printf("\n");
        currentIndex = endInd + 1;
    }
}
int isSpecialLine(const char *line) {
    return strcmp(line, "EndOfLine\n") && strcmp(line, "RightCurlyBracket\n");
}

int typeControl() { // 1 for intControl, 2 for StringControl.
    int temp = currentIndex;

    int isContinue1 = strcmp(lexLines[temp], "EndOfLine\n");
    int isContinue2 = strcmp(lexLines[temp], "RightCurlyBracket\n");
    while (isSpecialLine(lexLines[temp])) { // lexLines[temp] != "EndOfLine"
        if (strstr(lexLines[temp], "Keyword(int)")) {
            return 1;
        } else if (strstr(lexLines[temp], "Keyword(text)")) {
            return 2;
        }
        temp++;
    }
}

void bothInt(char lexLines[][300], int temp, int i) { // if both are integer identifiers.
    int toCopied = checkIntIndex(lexLines[temp + 1]);
    intVar[i] = intVar[toCopied];
}

void intControl() { // operands integer statements.
    int endInd = endIndex();
    int temp = currentIndex;
    if (lexLines[temp][0] == '\0') {
        exit(0);
    }
    int isExist = checkIntIndex(lexLines[temp]);

    if (typeControl() == 1 || isExist != -1) {

        while (strcmp(lexLines[temp], "EndOfLine\n")) {
            int k = checkLineIndex("Keyword(int)\n");
            int condition1 = k < temp && k != -1;
            int condition2 = k < temp;

            char str[100];
            strcpy(str, lexLines[temp]);
            char ident[100];
            toIdent(ident, str);
            int isNotSame = !isSameInt(ident) && !isSameTxt(ident);

            if (strstr(str, "Identifier") && condition1 && isNotSame) {

                strcpy(intIdentifiers[lastInt], ident);
                lastInt++;

            } else if (strcmp(str, "Keyword(is)\n") == 0 && condition2) {
                int control = intOperator(lexLines, temp);
                int i = checkIntIndex(lexLines[temp - 1]);
                if (strstr(lexLines[temp + 1], "Identifier") && control == -1) {
                    bothInt(lexLines, temp, i);
                } else if (control == -1) {
                    int value = toInt(lexLines[temp + 1]);
                    if (value > 99999999) {
                        printf("Max Integer value is 99999999! ");
                        value = 0;
                    }
                    intVar[i] = value;
                } else {
                    intVar[i] = control;
                }
            } else if (strcmp(str, "Comma\n") == 0) { // lexLines[temp] == "Comma"
                temp++;
                continue;
            }
            temp++;
        }

        currentIndex = endInd + 1;
    }
}
void bothStr(char lexLines[][300], int temp, int i) { // if both are text identifiers.
    int toCopied = checkStrIndex(lexLines[temp + 1]);
    strcpy(strNames[i], strNames[toCopied]);
}

void strControl() { // operands text statements.
    int endInd = endIndex();
    int temp = currentIndex;
    if (lexLines[temp][0] == '\0') {
        exit(0);
    }
    int isExist = checkStrIndex(lexLines[temp]);
    int isNotInt = checkIntIndex(lexLines[temp]);

    if (typeControl() == 2 || isExist != -1) {
        while (strcmp(lexLines[temp], "EndOfLine\n")) {

            int k = checkLineIndex("Keyword(text)\n");
            int condition1 = k < temp && k != -1;
            int condition2 = k < temp;

            char str[280];
            strcpy(str, lexLines[temp]);
            char ident[280];
            toIdent(ident, str);
            int isNotSame = !isSameInt(ident) && !isSameTxt(ident);

            if (strstr(str, "Identifier") && condition1 && isNotSame) {

                strcpy(strIdentifiers[lastStr], ident);
                lastStr++;

            } else if (strcmp(str, "Keyword(is)\n") == 0 && condition2) {

                int i = checkStrIndex(lexLines[temp - 1]);
                char operator= lexLines[temp + 2][9];
                if (operator== '+' || operator== '-') {
                    txtOperator(lexLines, temp);
                } else if (strstr(lexLines[temp + 1], "Identifier")) {
                    bothStr(lexLines, temp, i);
                } else {
                    toStr(str, lexLines[temp + 1]);
                    strcpy(strNames[i], str);
                }

            } else if (strcmp(lexLines[temp], "Comma\n") == 0 && condition2) {
                temp++;
                continue;
            }
            temp++;
        }

        currentIndex = endInd + 1;
    }
}

void interPreter() { // interpretes every statement.
    int i = 0;
    while (i < 1000) {
        oneRev();
        i++;
    }
}

int main() {

    fileReading("aa.sta");
    lexLinesCreate("aa.lex");
    interPreter();

    return 0;
}