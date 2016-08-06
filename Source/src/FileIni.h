
#ifndef OINI_FILE_INI_H
#define OINI_FILE_INI_H


#include "Globals.h"

#include "Poco/Util/IniFileConfiguration.h"


OINI_NAMESPACE_BEGIN(Oini)

class FileIni : public Poco::Util::AbstractConfiguration,
	private boost::noncopyable
{
public:
	FileIni();
	virtual ~FileIni();

protected:
	struct LineBounds
	{
		LineBounds(uint32 position, uint32 length) : Position(position),
			Length(length) {}

		uint32 Position;
		uint32 Length;
	};

protected:
	enum class LineMode : uint8
	{
		ORIGINAL,
		EDITED
	};

public:
	enum class LineType : uint8
	{
		EMPTY,
		COMMENT,
		SECTION,
		PROPERTY
	};

protected:
	struct Line
	{
		Line(uint32 position, uint32 length, LineType type) :
			Bounds(LineBounds(position, length)), Mode(LineMode::ORIGINAL),
			Type(type) {}

		LineBounds Bounds;
		LineMode Mode;
		LineType Type;
	};

public:
	void Initialize(const string& absolutePath);

public:
	string TextLine(uint32 index) const;
	uint32 TextLines() const;
	LineType TextLineType(uint32 index) const;

public:
	string TextLineName(uint32 index) const;
	string TextLineValue(uint32 index) const;

public:
	void NumberSignAsComment(bool set);

private:
	void enumerate(const std::string& key,
		Poco::Util::AbstractConfiguration::Keys& range) const override;
	bool getRaw(const std::string& key, std::string& value) const override;
	void setRaw(const std::string& key, const std::string& value) override;

public:
	string FileName() const { return mFileName; }
	string FullPath() const { return mFullPath; }
	string LineSeparator() const { return mLineSeparator; }

protected:
	string mFileName = "";
	string mFullPath = "";
	string mLineSeparator = "";
	string mOriginalText = "";

protected:
	std::vector<Line> mLines;
	std::vector<string> mStrings;
};

OINI_NAMESPACE_END(Oini)


#endif //OINI_FILE_INI_H
