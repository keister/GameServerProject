#pragma once

enum class PacketType : uint16
{

{%- for section in parser.sectionDict.values() %}
	{{section.name}} = {{section.begin}},
	{{' '}}
{%- for proto in section.listPacket %}
	//----------------------------------------------------------------
	/// {{proto.description}}\n
	///---------------------------------------------------------------
	{%- for params in proto.params %}
	/// - {{params.typeName}} {{params.name}}
	{%- endfor %}
	{%- for param in proto.params %}
		{%- for d in section.dto.dtos %}
			{%- if d.name == param.typeName or d.name == param.typeName.replace('vector<', '').replace('>', '') %}
	///	@ref {{d.name}}
				{%- for var in d.params %}
	///		- {{var.typeName}} {{var.name}}
				{%- endfor %}
			{%- endif %}
		{%- endfor %}
	{%- endfor %}
	///
	//----------------------------------------------------------------
	{{proto.name}}, 
	{{' '}}
	{%- endfor %}
	{{' '}}
{%- endfor %}
};



inline Packet& operator<< (Packet& pkt, PacketType type)
{
	pkt << static_cast<uint16>(type);

	return pkt;
}

inline Packet& operator>> (Packet& pkt, PacketType& type)
{
	pkt >> *reinterpret_cast<uint16*>(&type);

	return pkt;
}