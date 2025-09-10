"""XML parser for Vulkan registry."""

import xml.etree.ElementTree as ET
from dataclasses import dataclass
from enum import Enum
from typing import Dict, List, Optional
from pathlib import Path


class CommandType(Enum):
    """Vulkan command categorization."""
    GLOBAL = "global"
    INSTANCE = "instance" 
    DEVICE = "device"
    PHYSICAL_DEVICE = "physical_device"


@dataclass
class Parameter:
    """Function parameter definition."""
    name: str
    type_name: str
    is_pointer: bool = False
    is_const: bool = False
    array_length: Optional[str] = None


@dataclass
class Command:
    """Vulkan command definition."""
    name: str
    return_type: str
    parameters: List[Parameter]
    command_type: CommandType
    
    def get_first_handle_param(self) -> Optional[Parameter]:
        """Get the first handle parameter (dispatch source)."""
        for param in self.parameters:
            if param.type_name.startswith('Vk') and not param.is_pointer:
                return param
        return None


class VulkanRegistryParser:
    """Parses Vulkan XML registry to extract API definitions."""
    
    def __init__(self, xml_path: Path):
        self.xml_path = xml_path
        self._tree = ET.parse(xml_path)
        self._root = self._tree.getroot()
        self._commands: Dict[str, Command] = {}
        self._types: Dict[str, str] = {}
        self._parse_registry()
    
    def _parse_registry(self) -> None:
        """Parse the XML registry."""
        self._parse_types()
        self._parse_commands()
    
    def _parse_types(self) -> None:
        """Parse type definitions."""
        types_element = self._root.find('types')
        if types_element is None:
            return
            
        for type_elem in types_element.findall('type'):
            name_elem = type_elem.find('name')
            if name_elem is not None:
                self._types[name_elem.text] = type_elem.get('category', 'unknown')
    
    def _parse_commands(self) -> None:
        """Parse command definitions."""
        commands_element = self._root.find('commands')
        if commands_element is None:
            return
            
        for command_elem in commands_element.findall('command'):
            command = self._parse_command(command_elem)
            if command:
                self._commands[command.name] = command
    
    def _parse_command(self, command_elem: ET.Element) -> Optional[Command]:
        """Parse a single command definition."""
        proto = command_elem.find('proto')
        if proto is None:
            return None
            
        # Parse return type and name
        return_type = self._extract_type_text(proto)
        name_elem = proto.find('name')
        if name_elem is None:
            return None
        name = name_elem.text
        
        # Parse parameters
        parameters = []
        for param_elem in command_elem.findall('param'):
            param = self._parse_parameter(param_elem)
            if param:
                parameters.append(param)
        
        # Determine command type
        command_type = self._determine_command_type(name, parameters)
        
        return Command(name, return_type, parameters, command_type)
    
    def _parse_parameter(self, param_elem: ET.Element) -> Optional[Parameter]:
        """Parse a command parameter."""
        name_elem = param_elem.find('name')
        if name_elem is None:
            return None
            
        name = name_elem.text
        type_name = self._extract_type_text(param_elem)
        
        # Check for pointer and const modifiers
        param_text = ET.tostring(param_elem, encoding='unicode', method='text')
        is_pointer = '*' in param_text
        is_const = 'const' in param_text
        
        return Parameter(name, type_name, is_pointer, is_const)
    
    def _extract_type_text(self, element: ET.Element) -> str:
        """Extract type text from element, excluding name."""
        type_elem = element.find('type')
        return type_elem.text if type_elem is not None else 'void'
    
    def _determine_command_type(self, name: str, params: List[Parameter]) -> CommandType:
        """Determine the command type based on first parameter."""
        if not params:
            return CommandType.GLOBAL
            
        first_param_type = params[0].type_name
        
        if first_param_type == 'VkInstance':
            return CommandType.INSTANCE
        elif first_param_type == 'VkPhysicalDevice':
            return CommandType.PHYSICAL_DEVICE
        elif first_param_type == 'VkDevice' or first_param_type.startswith('Vk'):
            return CommandType.DEVICE
        
        return CommandType.GLOBAL
    
    def get_commands(self) -> Dict[str, Command]:
        """Get all parsed commands."""
        return self._commands
    
    def get_commands_by_type(self, command_type: CommandType) -> List[Command]:
        """Get commands filtered by type."""
        return [cmd for cmd in self._commands.values() 
                if cmd.command_type == command_type]
    
    def get_types(self) -> Dict[str, str]:
        """Get all parsed types."""
        return self._types