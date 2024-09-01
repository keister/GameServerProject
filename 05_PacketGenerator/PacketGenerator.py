from re import template
import argparse
import jinja2
import os

import ProtocolParser

if __name__ == '__main__' :
    arg_parser = argparse.ArgumentParser(description = 'IDLCompiler')
    arg_parser.add_argument('--path', type=str, default='D:/Portfolio/GameServerProject/Common/PacketGenerator/', help='proto path')
    args = arg_parser.parse_args()
    
    parser = ProtocolParser.Parser(args.path + 'protocol')
    parser.Parse()
 
    file_loader = jinja2.FileSystemLoader(args.path + 'Templates', encoding="cp949")
    env = jinja2.Environment(loader = file_loader)

    template = env.get_template('PacketDefine.h')
    output = template.render(parser = parser)
    
    f = open(parser.path, 'w+')
    f.write(output)
    f.close()

    for sec in parser.sectionDict.values() :
        groups = []
        servers = []
        dependencies = []
        for depen in parser.dependencies.values() :
            goto = False
            for proto in sec.listPacket :
                for var in proto.params :
                    if var.typeName in depen.structs :
                        goto = True
                        dependencies.append(depen)
                    if var.typeName in depen.classes :
                        goto = True
                        dependencies.append(depen)
                if goto == True :
                    break             

        for group in parser.groups.values() :
            if group.attach.section.name == sec.name :
                groups.append(group)
        for server in parser.servers.values() :
            if server.attach.section.name == sec.name :
                servers.append(server)
        template = env.get_template('handlePacket.h')
        output = template.render(
            primitive = ProtocolParser.primitive_types,
            section = sec, 
            groups = groups, 
            servers = servers,
            dependencies = dependencies)
        
        f = open(f'{sec.outputDir}{sec.outputName}.h', 'w+')
        f.write(output)
        f.close()
        
        template = env.get_template('handlePacket.cpp')
        output = template.render(
            primitive = ProtocolParser.primitive_types,
            section = sec, 
            groups = groups, 
            servers = servers,
            dependencies = dependencies,
            header = sec.outputName)
        
        f = open(f'{sec.outputDir}{sec.outputName}.cpp', 'w+')
        f.write(output)
        f.close()


    for group in parser.groups.values() :
        template = env.get_template('groupHandler.h')
        output = template.render(group = group, primitive = ProtocolParser.primitive_types)        
        writes = []
        onRead = True
        f = open(group.path);

        lines = f.readlines()
        textlines = []
        for line in lines :
            if '//@@@AutoPackBegin' in line :
                textlines.append(line)
                textlines.append('\n')
                textlines.append(output)
                onRead = False      
            elif '//@@@AutoPackEnd' in line :
                onRead = True

            if onRead == True :
                textlines.append(line)
        
        f.close()
        f = open(group.path, 'w+')
        
        for text in textlines:
            f.write(text)

        f.close()
        
    for group in parser.servers.values() :
        template = env.get_template('groupHandler.h')
        output = template.render(group = group, primitive = ProtocolParser.primitive_types)        
        writes = []
        onRead = True
        f = open(group.path);

        lines = f.readlines()
        textlines = []
        for line in lines :
            if '//@@@AutoPackBegin' in line :
                textlines.append(line)
                textlines.append(output)
                onRead = False      
            elif '//@@@AutoPackEnd' in line :
                onRead = True

            if onRead == True :
                textlines.append(line)
        
        f.close()
        f = open(group.path, 'w+')
        
        for text in textlines:
            f.write(text)

        f.close()



        
