
#ifndef OINI_FILE_INI_H
#define OINI_FILE_INI_H


#include "Globals.h"


NAMESPACE_BEGIN(Oini)

class FileIni : public Poco::Util::AbstractConfiguration,
	private boost::noncopyable
{
public:
	FileIni();
	virtual ~FileIni();

public:
	virtual void Initialize(const string& absolutePath);

public:
	virtual string TextLine(uint index) const;
	virtual uint TextLines() const;

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
	string mFileName;
	string mFullPath;
	string mLineSeparator;
	string mOriginalText;

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

protected:
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

protected:
	std::vector<Line> mLines;
	std::vector<string> mStrings;
};

NAMESPACE_END //Oini


#endif //OINI_FILE_INI_H
