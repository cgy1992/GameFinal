#include "ModelReader.h"
#include <shlwapi.h>
#include <iostream>

/*
	-f "p|t|b|n|tex
	-d "object_path_dir"
	-s scale
*/

void printHelpingInfo()
{
	std::cout << "The command: \n"
		<< "\t your_file_path -f vertex_format -d dest_dir -s scale\n"
		<< "\t NOTE:\n"
		<< "\t vertex_format : The elements you want to include in each vertex. \n\t Its format is 'p|n|t|b|tex'\n"
		<< "\t\t p: POSITION \n"
		<< "\t\t n: NORMAL \n"
		<< "\t\t t: TANGENT \n"
		<< "\t\t b: BINORMAL \n"
		<< "\t\t tex: TEXCOORD \n"
		<< "\t dest_dir: The directory to put your objective files \n"
		<< "\t scale: The vector scaling factor \n" << std::endl;
}

/*
EMVE_POSITION = 1,
EMVE_NORMAL = (1 << 1),
EMVE_TANGENT = (1 << 2),
EMVE_BINORMAL = (1 << 3),
EMVE_TEXCOORD = (1 << 4)
*/

bool parseVertexElementsParam(const char* str, u32& vertex_elements)
{
	if (str[0] == '-')
		return false;

	struct TextAndVertexElementPair
	{
		const char* text;
		E_MODEL_VERTEX_ELEMENT format;
	};

	const TextAndVertexElementPair pairs[] =
	{
		{ "p", EMVE_POSITION },
		{ "n", EMVE_NORMAL },
		{ "t", EMVE_TANGENT},
		{ "b", EMVE_BINORMAL},
		{ "tex", EMVE_TEXCOORD }
	};

	u32 i = 0, j = 0;
	vertex_elements = 0;
	while (1)
	{
		i++;
		if (str[i] == '|' || str[i] == '\0')
		{
			for (u32 k = 0; k < 5; k++)
			{
				if (strnicmp(&str[j], pairs[k].text, i - j) == 0)
				{
					vertex_elements |= pairs[k].format;
					break;
				}
			}

			if (str[i] == '\0')
				break;

			j = i + 1;
			i = j;
		}
	}

	// vertex formats must contain POSITION
	vertex_elements |= EMVE_POSITION;

	return true;
}

bool parseScaleParam(const char* str, f32& scale)
{
	if (str[0] == '-')
		return false;

	if (sscanf(str, "%f", &scale) > 0)
		return true;

	return false;
}

bool parseDestDirParam(const char* str, std::string& dest_dir)
{
	if (str[0] == '-')
		return false;

	if (!PathFileExistsA(str))
	{
		printf("'%s' is not a valid directory.\n", str);
		return false;
	}

	dest_dir = str;
	u32 len = strlen(str);
	if (str[len - 1] != '\\')
		dest_dir += std::string("\\");

	return true;
}


bool parseCommandLine(int argc, char *argv[],
	std::string& file_full_path,
	std::string& dest_dir,
	u32& vertex_elements,
	f32& scale)
{
	if (argc < 2 || stricmp("-h", argv[1]) == 0)
	{
		printHelpingInfo();
		return false;
	}

	const char* filename = argv[1];
	FILE* fp = fopen(filename, "r");
	if (!fp)
	{
		printf("The file '%s' doesn't exist.\n", filename);
		return false;
	}
	fclose(fp);

	file_full_path = filename;
	
	for (u32 i = 2; i < argc; i++)
	{
		if (stricmp(argv[i], "-f") == 0 && i < argc - 1)
		{
			if (parseVertexElementsParam(argv[i + 1], vertex_elements))
				i++;
		}
		else if (stricmp(argv[i], "-s") == 0 && i < argc - 1)
		{
			if (parseScaleParam(argv[i + 1], scale))
				i++;
		}
		else if (stricmp(argv[i], "-d") == 0 && i < argc - 1)
		{
			if (parseDestDirParam(argv[i + 1], dest_dir))
				i++;
			else
				dest_dir = "";
		}
	}

	return true;
}




