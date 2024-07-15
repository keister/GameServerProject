#pragma once
class Player;
class Packet;

{%- for depen in dependencies %}
{%- for st in depen.structs %}
struct {{st}};
{%- endfor %}

{%- for cl in depen.classes %}
class {{cl}};
{%- endfor %}
{%- endfor %}

{%- for group in groups %}
class {{group.name}};
{%- endfor %}

{%- for server in servers %}
class {{server.name}};
{%- endfor %}


{{' '}}


{%- for d in section.dto.dtos %}
//-------------------------------------------
{%- for var in d.params %}
/// - {{var.typeName}} {{var.name}}
{%- endfor %}
///
//-------------------------------------------
struct {{d.name}}
{
	{%- for var in d.params %}
	{{var.typeName}} {{var.name}};
	{%- endfor %}
};
{%- endfor %}


{{' '}}

{%- for d in section.dto.dtos %}
extern Packet& operator<< (Packet& pkt, const {{d.name}}& value);
extern Packet& operator>> (Packet& pkt, {{d.name}}& value);
{%- endfor %}

{%- for group in groups %}
extern bool HandlePacket_{{group.name}}({{group.name}}* group, Player& player, Packet& pkt);
{%- endfor %}

{%- for server in servers %}
extern bool HandlePacket_{{server.name}}({{server.name}}* server, Player& player, Packet& pkt);
{%- endfor %}


{%- for proto in section.dict_sc.values() %}
extern Packet& Make_{{proto.name}}({%- for var in proto.params %}const {{var.typeName}}{%- if var.typeName not in primitive %}&{%- endif %} {{var.name}}{%- if loop.index != proto.params|length %},{{' '}}{%- endif %}{%- endfor %});
{%- endfor %}