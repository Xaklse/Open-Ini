
#include "FileIni.h"
#include "Application.h"
#include "Exception.h"

#include "Poco/BinaryReader.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"


OINI_NAMESPACE_BEGIN(Oini)

FileIni::FileIni() : Poco::Util::AbstractConfiguration()
{
}

FileIni::~FileIni()
{
}

void FileIni::Initialize(const string& absolutePath)
{
	OINI_LOG("Trying to load file " + absolutePath);

	Poco::File file(absolutePath);

	if (!file.exists() || !file.isFile() || !file.canRead())
	{
		throw Exception("File " + absolutePath + " failed to load.");
	}

	const uint64 fileSize = file.getSize();

	OINI_LOG("Size in bytes of the file in the permanent storage: " +
		Str(fileSize));

	if (fileSize > 0u)
	{
		Poco::FileInputStream fileStream(absolutePath);
		Poco::BinaryReader fileReader(fileStream);

		fileReader.readRaw(fileSize, mOriginalText);
	}

	OINI_LOG("File successfully loaded; it takes " +
		Str(mOriginalText.capacity()) + " bytes of system memory.");

	mFileName = absolutePath.substr((absolutePath.find_last_of("/\\")) + 1);
	mFullPath = absolutePath;
	mLineSeparator = "\r\n";

	OINI_LOG("Parsing file...");

	if (fileSize > 0u)
	{
		string::const_iterator stringIterator = mOriginalText.begin();
		const string::const_iterator endIterator = mOriginalText.end();

		uint32 lineIndex = 0u;
		string lineText = "";
		LineType lineType = LineType::EMPTY;

		//Traverse the whole text of the file.
		while (stringIterator != endIterator)
		{
			char character = *stringIterator;

			if (character != '\r' && character != '\n')
			{
				if (lineType == LineType::EMPTY &&
					!Poco::Ascii::isSpace(character))
				{
					//Figure out the type of the line.
					if (character == ';')
					{
						lineType = LineType::COMMENT;
					}
					else if (character == '[')
					{
						lineType = LineType::SECTION;
					}
					else
					{
						lineType = LineType::PROPERTY;
					}
				}

				lineText += character;

				++stringIterator;
			}
			else
			{
				//Add a line of text to the dynamic array.
				mLines.push_back(Line(lineIndex, lineText.length(), lineType));

				lineIndex += lineText.length() + 1u;
				lineText = "";
				lineType = LineType::EMPTY;

				//Discard return-like characters.
				if (character == '\r')
				{
					mLineSeparator = "\r";
					++stringIterator;

					if (stringIterator != endIterator &&
						(*stringIterator) == '\n')
					{
						mLineSeparator += "\n";
						++stringIterator;

						++lineIndex;
					}
				}
				else if (character == '\n')
				{
					mLineSeparator = "\n";
					++stringIterator;
				}
			}
		}

		//Add the last line of text to the dynamic array.
		mLines.push_back(Line(lineIndex, lineText.length(), lineType));
	}

	OINI_LOG("File is parsed; it has " + Str(TextLines()) + " lines.");
}

string FileIni::TextLine(uint32 index) const
{
	if (mLines[index].Mode == LineMode::ORIGINAL)
	{
		return string(mOriginalText, mLines[index].Bounds.Position,
			mLines[index].Bounds.Length);
	}
	else
	{
		return string(mStrings[mLines[index].Bounds.Position]);
	}
}

uint32 FileIni::TextLines() const
{
	return mLines.size();
}

FileIni::LineType FileIni::TextLineType(uint32 index) const
{
	return mLines[index].Type;
}

string FileIni::TextLineName(uint32 index) const
{
	const string lineText = TextLine(index);
	const uint equalsSignPosition = lineText.find_first_of('=');

	if (equalsSignPosition == std::string::npos || equalsSignPosition == 0u)
	{
		return lineText;
	}

	string name = string(lineText, 0u, equalsSignPosition);
	Poco::trimInPlace(name);

	if (name.length() == 0u)
	{
		return lineText;
	}

	return name;
}

string FileIni::TextLineValue(uint32 index) const
{
	const string lineText = TextLine(index);
	const uint equalsSignPosition = lineText.find_first_of('=');

	if (equalsSignPosition == std::string::npos ||
		equalsSignPosition == lineText.length() - 1u)
	{
		return "";
	}

	string value = string(lineText, equalsSignPosition + 1u, lineText.length());
	Poco::trimInPlace(value);

	return value;
}

void FileIni::NumberSignAsComment(bool set)
{
	if (!set)
	{
		for (uint32 i = 0u; i < mLines.size(); ++i)
		{
			if (mLines[i].Type == FileIni::LineType::COMMENT &&
				TextLine(i)[0u] == '#')
			{
				mLines[i].Type = FileIni::LineType::PROPERTY;
			}
		}
	}
	else
	{
		for (uint32 i = 0u; i < mLines.size(); ++i)
		{
			if (mLines[i].Type == FileIni::LineType::PROPERTY &&
				TextLine(i)[0u] == '#')
			{
				mLines[i].Type = FileIni::LineType::COMMENT;
			}
		}
	}
}

void FileIni::enumerate(const std::string& key,
	Poco::Util::AbstractConfiguration::Keys& range) const
{
	throw Poco::NotImplementedException(-1);
}

bool FileIni::getRaw(const std::string& key, std::string& value) const
{
	return false;
}

void FileIni::setRaw(const std::string& key, const std::string& value)
{
	throw Poco::NotImplementedException(-1);
}


OINI_NAMESPACE_END(Oini)
