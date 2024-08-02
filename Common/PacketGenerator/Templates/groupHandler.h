	friend bool HandlePacket_{{group.name}}({{group.name}}*, Player&, Packet);

{%- for proto in group.attach.packets_cs %}
	void Handle_{{proto.name}}(Player& player{%- if proto.params|length != 0 %},{{' '}}{%- endif %}{%- for var in proto.params %}{{var.typeName}}{%- if var.typeName not in primitive %}&{%- endif %} {{var.name}}{%- if loop.index != proto.params|length %},{{' '}}{%- endif %}{%- endfor %});
{%- endfor %}
{{'\n'}}