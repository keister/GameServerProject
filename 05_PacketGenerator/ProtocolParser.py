import copy

primitive_types = [
    "bool", "char", "BYTE", "int8", "uint8", "int16", "uint16", "int32", "uint32",
    "int64", "uint64", "float32", "float64"
    ]

class Dto:
    def __init__(self, section) :
        self.section = section
        self.outputName  = ""
        self.dtos = []


    def Parse(self, block):
        idx = 0
        while idx < len(block):
            tokens = block[idx].split(' ')
            if tokens[0] == '@output_name':
                self.outputName = tokens[1].replace('\"', '')
            elif tokens[0] == '@link':
                link = Protocol('Dto_' + tokens[1])
                paramsblock = []
                idx+=2
                while block[idx].startswith(')') == False :
                    paramsblock.append(block[idx])
                    idx+=1

                link.Parse(paramsblock)
                self.dtos.append(link)
            idx+=1
                
                

class Varible:
    def __init__(self, typeName, name):
        self.typeName = typeName
        self.name = name

class Protocol:
    def __init__(self, name):
        self.name = name
        self.description = ""
        self.params = []
        
    def Parse(self, block):
        for i in block:
            tokens = i.split()
            self.params.append(Varible(tokens[0], tokens[1]))

        
class Section:
    def __init__(self, name):
        self.name = name
        self.dict_cs = {}
        self.dict_sc = {}
        self.listPacket = []
        self.begin = 0
        self.outputDir = ""
        self.outputName = ""
        self.dto = Dto(self)
        
    def Parse(self, block) :
        idx = 0
        
        while idx < len(block):
            tokens = block[idx].split(' ')
            if (tokens[0] == '@start'):
                self.begin = int(tokens[1])
            elif tokens[0] == '@output_dir' :
                self.outputDir = tokens[1].replace('\"', '')
            elif tokens[0] == '@output_name' :
                self.outputName = tokens[1].replace('\"', '')
            elif tokens[0] == '@protocol':
                proto = Protocol(tokens[1])
                idx+=1
                description = ""
                while block[idx].startswith("-") :
                    description += ' '.join(block[idx].replace("- ", "").split(' '))
                    idx+=1
                proto.description = copy.deepcopy(description)
                
                paramsblock = []
                idx+=1
                while block[idx].startswith(']') == False :
                    paramsblock.append(block[idx])
                    idx+=1

                proto.Parse(paramsblock)

                if tokens[1].startswith('C_') :
                    self.dict_cs[tokens[1]] = proto
                else :
                    self.dict_sc[tokens[1]] = proto    
                self.listPacket.append(proto)
                
            elif tokens[0] == '@dto':
                idx+=1
                
                dtoBlock = []
                while block[idx].startswith(']') == False :
                    dtoBlock.append(block[idx])
                    idx+=1

                self.dto.Parse(dtoBlock)
                

            idx+=1
                    


class Dependency :
    def __init__(self, name):
        self.name = name
        self.include = ""
        self.structs = []
        self.classes = []
    

    def Parse(self, block):
         idx = 0 
         while idx < len(block):
            tokens = block[idx].split(' ')
            if (tokens[0] == '@include'):
                self.include = tokens[1].replace('\"', '')
            elif tokens[0] == '@struct':
                idx += 2
                while block[idx].startswith(']') == False :
                    self.structs.append(block[idx].replace(',', ''))
                    idx+=1
            else :
                idx += 2
                while block[idx].startswith(']') == False :
                    self.classes.append(block[idx].replace(',', ''))
                    idx+=1
            idx+=1

class Attach :
    def __init__(self, section):
        self.section = section
        self.packets_sc = []
        self.packets_cs = []
        
    def AttachAll(self, parser):
        for val in self.section.dict_cs.values():
            self.packets_cs.append(val)
        for val in self.section.dict_sc.values():
            self.packets_sc.append(val)   
            

    def Parse(self, block):
        for i in block:
            i = i.replace(',', '')
            if i.startswith('S_'):
                self.packets_sc.append(self.section.dict_sc[i])
            else:
                self.packets_cs.append(self.section.dict_cs[i])
         

class Group :
    def __init__(self, name):
        self.name = name
        self.path = ""
        self.attach = Attach("")
        
    def Parse(self, block, parser):
        idx = 0
        while idx < len(block):
            tokens = block[idx].split(' ')
            if (tokens[0] == '@file_name'):
                self.path = tokens[1].replace('\"', '')
            elif tokens[0] == '@attach':
                self.attach = Attach(parser.sectionDict[tokens[1]])
                if (len(tokens) != 2) :
                    if (tokens[2] == 'all') :
                        self.attach.AttachAll(parser)
                        idx+=1
                        continue
                  
                idx += 2
                attachBlock = []
                while block[idx].startswith(']') == False :
                    attachBlock.append(block[idx])
                    idx+=1
                self.attach.Parse(attachBlock)
            idx+=1    

class Server :
    def __init__(self, name):
        self.name = name
        self.path = ""
        self.attach = Attach("")
        
    def Parse(self, block, parser):
        idx = 0
        while idx < len(block):
            tokens = block[idx].split(' ')
            if (tokens[0] == '@file_name'):
                self.path = tokens[1].replace('\"', '')
            elif tokens[0] == '@attach':
                self.attach = Attach(parser.sectionDict[tokens[1]])
                if (len(tokens) != 2) :
                    if (tokens[2] == 'all') :
                        self.attach.AttachAll(parser)
                        idx+=1
                        continue
                  
                idx += 2
                attachBlock = []
                while block[idx].startswith(']') == False :
                    attachBlock.append(block[idx])
                    idx+=1
                self.attach.Parse(attachBlock)
            idx+=1    
        

class Parser :
    def __init__(self, file):
        
        self.sectionDict = {}
        self.groups = {}
        self.servers = {}
        self.dependencies = {}
        self.path = ""
        f = open(file, 'r', encoding = 'UTF-8')
        lines  = f.readlines()
        textlines = []
        for line in lines :
            line = ' '.join(line.split())
            line = line.replace('\n', '')
            if (line != '') : 
                textlines.append(line)
        self.lines = textlines
        self.idx = 0;
    def Parse(self):
        
        goto = False
        idx = 0
        while idx < len(self.lines) :
            tokens = (self.lines)[idx].split(' ')
            if (tokens[0] == '@section'):
                self.sectionDict[tokens[1]] = Section(tokens[1])
                idx += 2
                block = []
                while self.lines[idx].startswith('}') == False:
                    block.append(self.lines[idx])
                    idx+=1  
                self.sectionDict[tokens[1]].Parse(block)
            elif tokens[0] == '@common_path' :
                self.path = tokens[1].replace('\"', '')
            elif tokens[0] == '@group' :
                self.groups[tokens[1]] = Group(tokens[1])
                idx += 2
                block = []
                while self.lines[idx].startswith('}') == False:
                    block.append(self.lines[idx])
                    idx+=1
                    
                self.groups[tokens[1]].Parse(block, self)
            elif tokens[0] == '@server' :
                self.servers[tokens[1]] = Server(tokens[1])
                idx += 2
                block = []
                while self.lines[idx].startswith('}') == False:
                    block.append(self.lines[idx])
                    idx+=1
                    
                self.servers[tokens[1]].Parse(block, self)            
            elif tokens[0] == '@dependency' :
                self.dependencies[tokens[1]] = Dependency(tokens[1])
                idx += 2
                block = []
                while self.lines[idx].startswith('}') == False:
                    block.append(self.lines[idx])
                    idx+=1
                    
                self.dependencies[tokens[1]].Parse(block)   
            idx+=1