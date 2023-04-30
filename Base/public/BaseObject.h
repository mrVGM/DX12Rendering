#pragma once

class BaseObjectMeta;

class BaseObject
{
private:
	const BaseObjectMeta& m_meta;
protected:
	BaseObject(const BaseObjectMeta& meta);
public:
	const BaseObjectMeta& GetMeta() const;
	bool CanCastTo(BaseObjectMeta& meta);

	virtual ~BaseObject();
};