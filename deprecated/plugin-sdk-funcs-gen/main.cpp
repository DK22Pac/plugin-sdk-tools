#include <conio.h>
#include <stdio.h>
#include <tchar.h>

#define MAX_FUNC_PARAMS 30
#define MAX_FUNCS 300

enum eFuncCallType
{
	FUNC_THISCALL,
	FUNC_STDCALL,
	FUNC_CDECL
};

struct tClassMember
{
	char comment[1024];
	char extraComment[512];
	char address[16];
	bool isStatic;

	tClassMember()
	{
		comment[0] = '\0';
		address[0] = '\0';
		isStatic = false;
	}
};

struct tFunctionParam{
	char typeName[32];
	char paramName[32];

	tFunctionParam()
	{
		typeName[0] = '\0';
		paramName[0] = '\0';
	}
};

struct tFunction : public tClassMember{
	char retTypeName[32];
	char className[64];
	char funcName[128];
	eFuncCallType funcType;
	unsigned char isMethod : 1;
	unsigned char isReturn : 1;
	unsigned char isConstructor : 1;
	unsigned char isDestructor : 1;
	tFunctionParam params[MAX_FUNC_PARAMS];
	unsigned char numParams;

	tFunction()
	{
		retTypeName[0] = '\0';
		className[0] = '\0';
		funcName[0] = '\0';
		isMethod = 0;
		isReturn = 0;
		isConstructor = 0;
		isDestructor = 0;
		numParams = 0;
	}
};

#define NUM_DEFAULT_TYPES 35

char *_defaultTypes[] = {"int", "signed int", "unsigned int", "uint", "bool", "short", "unsigned short",
	"char", "signed short", "signed char", "unsigned char", "Int32", "UInt32", "Int16", "UInt16", "Int8",
	"UInt8", "Bool", "Float", "float", "double", "Double", "__int8", "unsigned __int8", "signed __int8",
	"__int16", "unsigned __int16", "signed __int16", "__int32", "unsigned __int32", "signed __int32", "Long",
	"long", "ushort", "uchar"};

char _correctName[64];

char *_getTypeCorrectName(char *type)
{
	if(!strcmp(type, "uint"))
		strcpy(_correctName, "unsigned int");
	else if(!strcmp(type, "ushort"))
		strcpy(_correctName, "unsigned short");
	else if(!strcmp(type, "uchar"))
		strcpy(_correctName, "unsigned char");
	else if(!strcmp(type, "uint*"))
		strcpy(_correctName, "unsigned int*");
	else if(!strcmp(type, "ushort*"))
		strcpy(_correctName, "unsigned short*");
	else if(!strcmp(type, "uchar*"))
		strcpy(_correctName, "unsigned char*");
	else if(!strcmp(type, "uint *"))
		strcpy(_correctName, "unsigned int*");
	else if(!strcmp(type, "ushort *"))
		strcpy(_correctName, "unsigned short*");
	else if(!strcmp(type, "uchar *"))
		strcpy(_correctName, "unsigned char*");
	else if(!strcmp(type, "uint**"))
		strcpy(_correctName, "unsigned int**");
	else if(!strcmp(type, "ushort**"))
		strcpy(_correctName, "unsigned short**");
	else if(!strcmp(type, "uchar**"))
		strcpy(_correctName, "unsigned char**");
	else if(!strcmp(type, "uint **"))
		strcpy(_correctName, "unsigned int **");
	else if(!strcmp(type, "ushort **"))
		strcpy(_correctName, "unsigned short **");
	else if(!strcmp(type, "uchar **"))
		strcpy(_correctName, "unsigned char **");
	else if(!strcmp(type, "uint&"))
		strcpy(_correctName, "unsigned int&");
	else if(!strcmp(type, "ushort&"))
		strcpy(_correctName, "unsigned short&");
	else if(!strcmp(type, "uchar&"))
		strcpy(_correctName, "unsigned char&");
	else if(!strcmp(type, "uint &"))
		strcpy(_correctName, "unsigned int &");
	else if(!strcmp(type, "ushort &"))
		strcpy(_correctName, "unsigned short &");
	else if(!strcmp(type, "uchar &"))
		strcpy(_correctName, "unsigned char &");
	else
		strcpy(_correctName, type);
	return _correctName;
}

tFunction functions[MAX_FUNCS];
unsigned int numFunctions = 0;
unsigned int numVtableFunctions = 0;

void _skipSpace(char* line, int &counter)
{
	while (line[counter] == ' ' || line[counter] == '	')
		counter++;
}

int _getNumCharacters(char *str, char character)
{
	int i = 0;
	int count = 0;
	while (str[i])
	{
		if (str[i] == character)
			count++;
		i++;
	}
	return count;
}

int _readToCharacter(char *line, int &counter, char* buf, char character)
{
	int i = 0;
	while (line[counter] != character && line[counter] != '\0')
	{
		buf[i] = line[counter];
		i++;
		counter++;
	}
	counter++;
	buf[i] = '\0';
	return i;
}

bool _hasCharacter(char* str, char character)
{
	int i = 0;
	while (str[i])
	{
		if (str[i] == character)
			return true;
		i++;
	}
	return false;
}

bool _isTypeDefault(char *type)
{
	if(_hasCharacter(type, '*') || _hasCharacter(type, '&'))
		return true;
	for(int i = 0; i < NUM_DEFAULT_TYPES; i++)
	{
		if(!strcmp(type, _defaultTypes[i]))
			return true;
	}
	return false;
}

void _dealWithParameter(tFunctionParam& param, int paramId, char* str) {
	int Istr = 0;
	_skipSpace(str, Istr);
	int i = 0;
	while (str[Istr] != ' ' && str[Istr] != '*' && str[Istr] != '&' && str[Istr] != '\0')
	{
		param.typeName[i] = str[Istr];
		i++;
		Istr++;
	}
	param.typeName[i] = '\0';
	sprintf(param.paramName, "arg%d", paramId);
	if (str[Istr] == '\0')
		return;
	_skipSpace(str, Istr);
	while (str[Istr] != '\0') {
		if (str[Istr] == '&' || str[Istr] == '*') {
			strncat(param.typeName, &str[Istr], 1);
			Istr++;
		}
		else if (!strncmp(&str[Istr], "const", 5)) {
			strcat(param.typeName, " const");
			Istr += 5;
		}
		else {
			_readToCharacter(str, Istr, param.paramName, ' ');
			break;
		}
		_skipSpace(str, Istr);
	}
}

void _dealWithParameter2(tFunctionParam& param, int paramId, char* str)
{
	int Istr = 0;
	char arg[8];
	_skipSpace(str, Istr);
	if(!_hasCharacter(&str[Istr], ' '))
	{
		strcpy(param.typeName, &str[Istr]);
		sprintf(arg, "arg%d", paramId);
		strcpy(param.paramName, arg);
		return;
	}
	_readToCharacter(str, Istr, param.typeName, ' ');
	_skipSpace(str, Istr);
	if(str[Istr] == '\0')
	{
		sprintf(arg, "arg%d", paramId);
		strcpy(param.paramName, arg);
		return;
	}
	if(!strncmp(&str[Istr], "const", 5))
	{
		strcat(param.typeName, " const");
		Istr += 5;
	}
	_skipSpace(str, Istr);
	if(str[Istr] == '*')
	{
		strcat(param.typeName, "*");
		Istr++;
	}
	if(str[Istr] == '&')
	{
		strcat(param.typeName, "&");
		Istr++;
	}
	if(str[Istr] == '*')
	{
		strcat(param.typeName, "*");
		Istr++;
	}
	if(str[Istr] == '&')
	{
		strcat(param.typeName, "&");
		Istr++;
	}
	if(str[Istr] == '\0')
	{
		sprintf(arg, "arg%d", paramId);
		strcpy(param.paramName, arg);
		return;
	}
	int Idummy = 0;
	_readToCharacter(&str[Istr], Idummy, param.paramName, ' ');
}

char _extraComment[512];

char *_normaliseExtraComment(char *str)
{
	int i, j = 0;
	for(i = 0; i < 512; i++)
	{
		if(str[i] == '\0' || str[i] == '\n')
			break;
		if((str[i] == ' ' || str[i] == '	') && (str[i + 1] == ' ' || str[i + 1] == '	'))
			continue;
		_extraComment[j] = str[i];
		j++;
	}
	_extraComment[j] = '\0';
	return _extraComment;
}

int _tmain(int argc, _TCHAR* argv[])
{
    bool oldStyle = false;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (_stricmp(argv[i], "-oldstyle")) {
                oldStyle = true;
                break;
            }
        }
    }
	FILE *f = fopen("funcs.h", "r");
	char dummy[256];
	char parameter[128];
	char line[512];
	int vtable_base = 0;
	int Iline, Idummy;
	int numParams;
	memset(line, 0, 512);
	fgets(line, 512, f);
	sscanf(line, "%s %d", dummy, &vtable_base);
	memset(line, 0, 512);
	fgets(line, 512, f);

    // read vtable... end
	while (strncmp(line, "end", 3))
	{
		Iline = 0;

		// get comment
		while (line[0] == '/')
		{
			strcat(functions[numFunctions].comment, line);
			memset(line, 0, 512);
			fgets(line, 512, f);
		}

		printf("VTABLE %s \n\n", line);
		strcpy(functions[numFunctions].extraComment, line);

		functions[numFunctions].funcType = FUNC_THISCALL;
		// read func ret type
		_skipSpace(line, Iline);
		_readToCharacter(line, Iline, dummy, ' ');
		if(strcmp(dummy, "void"))
			functions[numFunctions].isReturn = 1;
		strcpy(functions[numFunctions].retTypeName, dummy);
		// skip ' '
		_skipSpace(line, Iline);
		// read function name
		_readToCharacter(line, Iline, dummy, '(');
		Idummy = 0;
		_readToCharacter(dummy, Idummy, functions[numFunctions].className, ':');
		Idummy++;
		strcpy(functions[numFunctions].funcName, &dummy[Idummy]);
		if (_hasCharacter(functions[numFunctions].funcName, '~'))
			functions[numFunctions].isDestructor = true;
		else if (!strcmp(functions[numFunctions].funcName, functions[numFunctions].className))
			functions[numFunctions].isConstructor = true;
		functions[numFunctions].isMethod = true;
		// read parameters
		_readToCharacter(line, Iline, dummy, ')');
		if(dummy[0] != '\0' && strcmp(dummy, "void"))
		{
			Idummy = 0;
			numParams = _getNumCharacters(dummy, ',');
			for(int i = 0; i < numParams; i++)
			{
				_readToCharacter(dummy, Idummy, parameter, ',');
				_dealWithParameter(functions[numFunctions].params[functions[numFunctions].numParams], 
					functions[numFunctions].numParams, parameter);
				functions[numFunctions].numParams++;
			}
			_dealWithParameter(functions[numFunctions].params[functions[numFunctions].numParams], 
				functions[numFunctions].numParams, &dummy[Idummy]);
			functions[numFunctions].numParams++;
		}

		sprintf(dummy, "%d", vtable_base + numFunctions);
		strcpy(functions[numFunctions].address, dummy);
		
		printf("Function info:\n retTypeName %s\n className %s\n funcName %s\n funcType %d\n isMethod %d\n isStatic %d\n isReturn %d\n isConstructor %d\n isDestructor %d\n address %s\n",
			functions[numFunctions].retTypeName, 
			functions[numFunctions].className,
			functions[numFunctions].funcName,
			functions[numFunctions].funcType,
			functions[numFunctions].isMethod,
			functions[numFunctions].isStatic,
			functions[numFunctions].isReturn,
			functions[numFunctions].isConstructor,
			functions[numFunctions].isDestructor,
			functions[numFunctions].address);
		printf("  Num params: %d\n", functions[numFunctions].numParams);
		for(int i = 0; i < functions[numFunctions].numParams; i++)
			printf("    Param %s with_type %s\n", functions[numFunctions].params[i].paramName, functions[numFunctions].params[i].typeName);
		printf("Comment: %s\n\n", functions[numFunctions].comment);

		numVtableFunctions++;
		numFunctions++;

		memset(line, 0, 512);
		fgets(line, 512, f);
	}

	// read funcs...end
	
	fgets(line, 512, f);
	memset(line, 0, 512);
	fgets(line, 512, f);
	while (strncmp(line, "end", 3))
	{
		Iline = 0;

		// get comment
		while (line[0] == '/')
		{
			strcat(functions[numFunctions].comment, line);
			memset(line, 0, 512);
			fgets(line, 512, f);
		}

		printf("FUNC %s \n\n", line);
		strcpy(functions[numFunctions].extraComment, line);

		_skipSpace(line, Iline);
		_readToCharacter(line, Iline, dummy, ' ');
		_skipSpace(line, Iline);
		if(!strcmp(dummy, "thiscall"))
			functions[numFunctions].funcType = FUNC_THISCALL;
		else if(!strcmp(dummy, "stdcall"))
			functions[numFunctions].funcType = FUNC_STDCALL;
		else
			functions[numFunctions].funcType = FUNC_CDECL;
		// read func ret type
		_readToCharacter(line, Iline, dummy, ' ');
		if(strcmp(dummy, "void"))
			functions[numFunctions].isReturn = 1;
		strcpy(functions[numFunctions].retTypeName, dummy);
		// skip ' '
		_skipSpace(line, Iline);
		// read function name
		_readToCharacter(line, Iline, dummy, '(');
		Idummy = 0;
		if(_hasCharacter(dummy, ':'))
		{
			_readToCharacter(dummy, Idummy, functions[numFunctions].className, ':');
			Idummy++;
			strcpy(functions[numFunctions].funcName, &dummy[Idummy]);
			functions[numFunctions].isMethod = true;
			if(_hasCharacter(functions[numFunctions].funcName, '~'))
				functions[numFunctions].isDestructor = true;
			else if(!strcmp(functions[numFunctions].funcName, functions[numFunctions].className))
				functions[numFunctions].isConstructor = true;
			if(functions[numFunctions].funcType != FUNC_THISCALL)
				functions[numFunctions].isStatic = true;
		}
		else
			strcpy(functions[numFunctions].funcName, dummy);
		// read parameters
		_readToCharacter(line, Iline, dummy, ')');
		if(dummy[0] != '\0' && strcmp(dummy, "void"))
		{
			Idummy = 0;
			numParams = _getNumCharacters(dummy, ',');
			for(int i = 0; i < numParams; i++)
			{
				_readToCharacter(dummy, Idummy, parameter, ',');
				_dealWithParameter(functions[numFunctions].params[functions[numFunctions].numParams], 
					functions[numFunctions].numParams, parameter);
				functions[numFunctions].numParams++;
			}
			_dealWithParameter(functions[numFunctions].params[functions[numFunctions].numParams], 
				functions[numFunctions].numParams, &dummy[Idummy]);
			functions[numFunctions].numParams++;
		}
		_skipSpace(line, Iline);
		strncpy(functions[numFunctions].address, &line[Iline], 8);
		
		printf("Function info:\n retTypeName %s\n className %s\n funcName %s\n funcType %d\n isMethod %d\n isStatic %d\n isReturn %d\n isConstructor %d\n isDestructor %d\n address %s\n",
			functions[numFunctions].retTypeName, 
			functions[numFunctions].className,
			functions[numFunctions].funcName,
			functions[numFunctions].funcType,
			functions[numFunctions].isMethod,
			functions[numFunctions].isStatic,
			functions[numFunctions].isReturn,
			functions[numFunctions].isConstructor,
			functions[numFunctions].isDestructor,
			functions[numFunctions].address);
		printf("  Num params: %d\n", functions[numFunctions].numParams);
		for(int i = 0; i < functions[numFunctions].numParams; i++)
			printf("    Param %s with_type %s\n", functions[numFunctions].params[i].paramName, functions[numFunctions].params[i].typeName);
		printf("\n");
		numFunctions++;

		memset(line, 0, 512);
		fgets(line, 512, f);
	}

	fclose(f);

	// now flush data into new file

	f = fopen("funcs_conv.h", "w");
	fputs("// header\n\n", f);
	if(numVtableFunctions > 0)
	{
		if(functions[0].isMethod)
			fputs("    ", f);
		fputs("//vtable\n\n", f);
	}
	for(unsigned int i = 0; i < numFunctions; i++)
	{
		if(numVtableFunctions == i)
		{
			fputs("\n", f);
			if(functions[i].isMethod)
				fputs("    ", f);
			fputs("//funcs\n\n", f);
		}
		if(functions[i].comment[0] != '\0')
		{
			if(functions[i].isMethod)
				fputs("    ", f);
			fputs(functions[i].comment, f);
		}
		if(functions[i].isMethod)
			fputs("    ", f);
		if(functions[i].isStatic)
			fputs("static ", f);
		if(!functions[i].isConstructor && !functions[i].isDestructor)
		{
			fputs(_getTypeCorrectName(functions[i].retTypeName), f);
			fputs(" ", f);
		}
		fputs(functions[i].funcName, f);
		fputs("(", f);
		if(functions[i].numParams > 0)
		{
			for(int j = 0; j < functions[i].numParams - 1; j++)
			{
				fputs(_getTypeCorrectName(functions[i].params[j].typeName), f);
				fputs(" ", f);
				fputs(functions[i].params[j].paramName, f);
				fputs(", ", f);
			}
			fputs(_getTypeCorrectName(functions[i].params[functions[i].numParams - 1].typeName), f);
			fputs(" ", f);
			fputs(functions[i].params[functions[i].numParams - 1].paramName, f);
			fputs(");\n", f);
		}
		else
			fputs(");\n", f);
	}

	// source

	fputs("\n// source\n\n", f);

	for(unsigned int i = 0; i < numFunctions; i++)
	{
		fputs("// Converted from ", f);
		fputs(_normaliseExtraComment(functions[i].extraComment), f);
		fputs("\n", f);
		if(!functions[i].isConstructor && !functions[i].isDestructor)
		{
			fputs(_getTypeCorrectName(functions[i].retTypeName), f);
			fputs(" ", f);
		}
		if(functions[i].isMethod)
		{
			fputs(functions[i].className, f);
			fputs("::", f);
		}
		fputs(functions[i].funcName, f);
		fputs("(", f);
		if(functions[i].numParams > 0)
		{
			for(int j = 0; j < functions[i].numParams - 1; j++)
			{
				fputs(_getTypeCorrectName(functions[i].params[j].typeName), f);
				fputs(" ", f);
				fputs(functions[i].params[j].paramName, f);
				fputs(", ", f);
			}
			fputs(_getTypeCorrectName(functions[i].params[functions[i].numParams - 1].typeName), f);
			fputs(" ", f);
			fputs(functions[i].params[functions[i].numParams - 1].paramName, f);
			fputs(")", f);
		}
		else
			fputs(")", f);
		fputs(" {\n    ", f);

		if(functions[i].isReturn)
		{
			if(!_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
			{
				fputs(_getTypeCorrectName(functions[i].retTypeName), f);
				fputs(" result;\n    ", f);
			}
			else
				fputs("return ", f);
		}
        bool hasParameters = false;
        if (oldStyle) {
            switch (functions[i].funcType)
            {
            case FUNC_THISCALL:
                hasParameters = true;
                if (functions[i].isReturn) {
                    if (_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                        fprintf(f, "reinterpret_cast<%s(__thiscall *)(%s *", _getTypeCorrectName(functions[i].retTypeName), functions[i].className);
                    else
                        fprintf(f, "reinterpret_cast<void(__thiscall *)(%s *", functions[i].className);
                }
                else
                    fprintf(f, "reinterpret_cast<void(__thiscall *)(%s *", functions[i].className);
                break;
            case FUNC_CDECL:
                if (functions[i].isReturn) {
                    if (_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                        fprintf(f, "reinterpret_cast<%s(__cdecl *)(", _getTypeCorrectName(functions[i].retTypeName));
                    else
                        fprintf(f, "reinterpret_cast<void(__cdecl *)(");
                }
                else
                    fprintf(f, "reinterpret_cast<void(__cdecl *)(");
            }

            if (functions[i].isReturn)
            {
                if (!_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                {
                    if (hasParameters) {
                        fputs(", ", f);
                    }
                    else {
                        hasParameters = true;
                    }
                    fputs(_getTypeCorrectName(functions[i].retTypeName), f);
                    fputs("*", f);
                }
            }
        }
        else {
            hasParameters = true;
            switch (functions[i].funcType)
            {
            case FUNC_THISCALL:
                if (i < numVtableFunctions) {
                    if (functions[i].isReturn) {
                        if (_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                            fprintf(f, "plugin::CallVirtualMethodAndReturn<%s, %s, %s *", _getTypeCorrectName(functions[i].retTypeName), functions[i].address, functions[i].className);
                        else
                            fprintf(f, "plugin::CallVirtualMethod<%s, %s *", functions[i].address, functions[i].className);
                    }
                    else
                        fprintf(f, "plugin::CallVirtualMethod<%s, %s *", functions[i].address, functions[i].className);
                }
                else {
                    if (functions[i].isReturn) {
                        if (_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                            fprintf(f, "plugin::CallMethodAndReturn<%s, %s, %s *", _getTypeCorrectName(functions[i].retTypeName), functions[i].address, functions[i].className);
                        else
                            fprintf(f, "plugin::CallMethod<%s, %s *", functions[i].address, functions[i].className);
                    }
                    else
                        fprintf(f, "plugin::CallMethod<%s, %s *", functions[i].address, functions[i].className);
                }
                break;
            case FUNC_CDECL:
                if (functions[i].isReturn) {
                    if (_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                        fprintf(f, "plugin::CallAndReturn<%s, %s", _getTypeCorrectName(functions[i].retTypeName), functions[i].address);
                    else
                        fprintf(f, "plugin::Call<%s", functions[i].address);
                }
                else
                    fprintf(f, "plugin::Call<%s", functions[i].address);
            }

            if (functions[i].isReturn)
            {
                if (!_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
                {
                    fputs(", ", f);
                    fputs(_getTypeCorrectName(functions[i].retTypeName), f);
                    fputs("*", f);
                }
            }
        }

		if (functions[i].numParams > 0)
		{
            if (hasParameters) {
                fputs(", ", f);
            }
			for (int j = 0; j < functions[i].numParams - 1; j++)
			{
				fputs(_getTypeCorrectName(functions[i].params[j].typeName), f);
				fputs(", ", f);
			}
			fputs(_getTypeCorrectName(functions[i].params[functions[i].numParams - 1].typeName), f);
		}

        if (oldStyle) {
            fputs(")>(", f);
            if (i < numVtableFunctions) {
                fprintf(f, "(*reinterpret_cast<void ***>(this))[%s])(", functions[i].address);
            }
            else {
                fprintf(f, "%s)(", functions[i].address);
            }
        }
        else {
            fputs(">(", f);
        }

		if (functions[i].funcType == FUNC_THISCALL)
			fputs("this", f);

		if(functions[i].isReturn)
		{
			if(!_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
			{
				if(functions[i].funcType == FUNC_THISCALL)
					fputs(", ", f);
				fputs("&result", f);
			}
		}

		if(functions[i].numParams > 0)
		{
			if(functions[i].funcType == FUNC_THISCALL || (functions[i].isReturn && !_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName))))
				fputs(", ", f);
			for(int j = 0; j < functions[i].numParams - 1; j++)
			{
				fputs(functions[i].params[j].paramName, f);
				fputs(", ", f);
			}
			fputs(functions[i].params[functions[i].numParams - 1].paramName, f);
			fputs(");\n", f);
		}
		else
			fputs(");\n", f);

		if(functions[i].isReturn)
		{
			if(!_isTypeDefault(_getTypeCorrectName(functions[i].retTypeName)))
			{
				fputs("    return result;\n", f);
			}
		}

		fputs("}\n\n", f);
	}

	fclose(f);

	_getch();

	return 0;
}