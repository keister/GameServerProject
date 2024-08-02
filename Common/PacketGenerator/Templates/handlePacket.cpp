#include "stdafx.h"
#include "{{header}}.h"

#include "Player.h"
#include "Packet.h"

{%- for group in groups %}
#include "{{group.name}}.h"
{%- endfor %}
{%- for server in servers %}
#include "{{server.name}}.h"
{%- endfor %}


#include "Common/PacketDefine.h"

{%- for depen in dependencies %}
#include "{{depen.include}}"
{%- endfor %}



{%- for d in section.dto.dtos %}
RawPacket& operator<< (RawPacket& pkt, const {{d.name}}& value)
{

	pkt << {%- for var in d.params %} value.{{var.name}}{%- if loop.index != d.params|length %} << {%- endif %} {%- endfor %};

	return pkt;
}
{%- endfor %}

{%- for d in section.dto.dtos %}
RawPacket& operator>> (RawPacket& pkt, {{d.name}}& value)
{

	pkt << {%- for var in d.params %} value.{{var.name}}{%- if loop.index != d.params|length %} >> {%- endif %} {%- endfor %};

	return pkt;
}
{%- endfor %}


{%- for group in groups %}
bool HandlePacket_{{group.name}}({{group.name}}* group, Player& player, Packet pkt)
{
	PacketType type;
	*pkt >> type;

	switch (type)
	{
		{%- for proto in group.attach.packets_cs %}
	case PacketType::{{proto.name}}:
	{
		{%- for var in proto.params %}
		{{var.typeName}} {{var.name}};
		{%- endfor %}
		{%- if proto.params|length != 0 %}
		*pkt >> {%- for var in proto.params %} {{var.name}}{%- if loop.index != proto.params|length %} >> {%- endif %} {%- endfor %};
		{%- endif %}

		group->Handle_{{proto.name}}(player{%- if proto.params|length != 0 %},{{' '}}{%- endif %}{%- for var in proto.params %}{{var.name}}{%- if loop.index != proto.params|length %},{{' '}}{%- endif %} {%- endfor %});
		break;
	}
	{%- endfor %}
	default:
		return false;
	}

	return true;
}
{%- endfor %}

{%- for server in servers %}
bool HandlePacket_{{server.name}}({{server.name}}* server, Player& player, Packet pkt)
{
	PacketType type;
	*pkt >> type;

	switch (type)
	{
		{%- for proto in server.attach.packets_cs %}
	case PacketType::{{proto.name}}:
	{
		{%- for var in proto.params %}
		{{var.typeName}} {{var.name}};
		{%- endfor %}
		{%- if proto.params|length != 0 %}
		*pkt >> {%- for var in proto.params %} {{var.name}}{%- if loop.index != proto.params|length %} >> {%- endif %} {%- endfor %};
		{%- endif %}

		server->Handle_{{proto.name}}(player{%- if proto.params|length != 0 %},{{''}}{%- endif %}{%- for var in proto.params %}{{var.name}}{%- if loop.index != proto.params|length %},{{' '}}{%- endif %} {%- endfor %});
		break;
	}
	{%- endfor %}
	default:
		return false;
	}

	return true;
}
{%- endfor %}

{%- for proto in section.dict_sc.values() %}
Packet Make_{{proto.name}}({%- for var in proto.params %}const {{var.typeName}}{%- if var.typeName not in primitive %}&{%- endif %} {{var.name}}{%- if loop.index != proto.params|length %},{{' '}}{%- endif %}{%- endfor %})
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::{{proto.name}};
	*pkt << {%- for var in proto.params%} {{var.name}}{%- if loop.index != proto.params|length %} << {%- endif %} {%- endfor %};

	return pkt;
}
{%- endfor %}