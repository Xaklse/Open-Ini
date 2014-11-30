
#include "FileIni.h"
#include "Application.h"
#include "Exception.h"

#include "Poco/BinaryReader.h"
#include "Poco/File.h"
#include "Poco/FileStream.h"


NAMESPACE_BEGIN(Oini)

FileIni::FileIni() : Poco::Util::AbstractConfiguration(),
	mFileName(""),
	mFullPath(""),
	mOriginalText("")
{
}

FileIni::~FileIni()
{
}

/*virtual*/
void FileIni::Initialize(const string& absolutePath)
{
	LOG("Trying to load file " + absolutePath);

	Poco::File file(absolutePath);

	if (!file.exists() || !file.isFile() || !file.canRead())
	{
		throw Exception("File " + absolutePath + " failed to load.");
	}

	const uint32 fileSize = file.getSize();

	LOG("Size in bytes of the file in the permanent storage: " + Str(fileSize));

	if (fileSize > 0)
	{
		Poco::FileInputStream fileStream(absolutePath);
		Poco::BinaryReader fileReader(fileStream);

		fileReader.readRaw(fileSize, mOriginalText);
	}

	LOG("File successfully loaded; it takes " + Str(mOriginalText.capacity()) +
		" bytes of system memory.");

	mFileName = absolutePath.substr((absolutePath.find_last_of("/\\")) + 1);
	mFullPath = absolutePath;
	mLineSeparator = "\r\n";

	LOG("Parsing file...");

	if (fileSize > 0)
	{
		string::const_iterator stringIterator = mOriginalText.begin();
		const string::const_iterator endIterator = mOriginalText.end();

		uint32 lineIndex = 0;
		string lineText = "";
		LineType lineType = LineType::EMPTY;

		//Traverse the whole text of the file.
		while (stringIterator != endIterator)
		{
			int charCode = *stringIterator;

			if (charCode != '\r' && charCode != '\n')
			{
				if (lineType == LineType::EMPTY &&
					!Poco::Ascii::isSpace(charCode))
				{
					//Figure out the type of the line.
					if (charCode == ';')
					{
						lineType = LineType::COMMENT;
					}
					else if (charCode == '[')
					{
						lineType = LineType::SECTION;
					}
					else
					{
						lineType = LineType::PROPERTY;
					}
				}

				lineText += charCode;

				stringIterator++;
			}
			else
			{
				//Add a line of text to the dynamic array.
				mLines.push_back(Line(lineIndex, lineText.length(), lineType));

				lineIndex += lineText.length() + 1;
				lineText = "";
				lineType = LineType::EMPTY;

				//Discard return-like characters.
				if (charCode == '\r')
				{
					mLineSeparator = "\r";
					stringIterator++;

					if (stringIterator != endIterator &&
						(*stringIterator) == '\n')
					{
						mLineSeparator += "\n";
						stringIterator++;

						lineIndex++;
					}
				}
				else if (charCode == '\n')
				{
					mLineSeparator = "\n";
					stringIterator++;
				}
			}
		}

		//Add the last line of text to the dynamic array.
		mLines.push_back(Line(lineIndex, lineText.length(), lineType));
	}

	LOG("File is parsed; it has " + Str(TextLines()) + " lines.");
}

/*virtual*/
string FileIni::TextLine(uint index) const
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

/*virtual*/
uint FileIni::TextLines() const
{
	return mLines.size();
}

void FileIni::enumerate(const std::string& key,
	Poco::Util::AbstractConfiguration::Keys& range) const
{
}

bool FileIni::getRaw(const std::string& key, std::string& value) const
{
	return true;
}

void FileIni::setRaw(const std::string& key, const std::string& value)
{
}


NAMESPACE_END //Oini
