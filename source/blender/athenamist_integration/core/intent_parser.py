"""
Blender AI Agent - Intent Parser
================================

This module provides natural language understanding for the Blender AI Agent.
It parses user input and converts it into actionable intents that can be
executed by the workflow engine.
"""

import re
import json
from typing import Dict, List, Any, Optional
from dataclasses import dataclass
from enum import Enum

import bpy
import bmesh


class IntentType(Enum):
    """Types of user intents that can be recognized."""
    CREATE_OBJECT = "create_object"
    MODIFY_OBJECT = "modify_object"
    CREATE_MATERIAL = "create_material"
    ANIMATE_OBJECT = "animate_object"
    OPTIMIZE_SCENE = "optimize_scene"
    RENDER_SCENE = "render_scene"
    APPLY_MODIFIER = "apply_modifier"
    SETUP_LIGHTING = "setup_lighting"
    CREATE_TEXTURE = "create_texture"
    HELP_REQUEST = "help_request"


@dataclass
class Intent:
    """Represents a parsed user intent."""
    type: IntentType
    confidence: float
    parameters: Dict[str, Any]
    context: Dict[str, Any]
    suggestions: List[str]


class IntentParser:
    """
    Natural language intent parser for Blender AI Agent.
    
    This class understands user input and converts it into actionable
    intents that can be executed by the workflow engine.
    """
    
    def __init__(self):
        """Initialize the intent parser with patterns and models."""
        self.patterns = self._load_patterns()
        self.context_analyzer = ContextAnalyzer()
        self.intent_classifier = IntentClassifier()
    
    def parse(self, user_input: str, context: Dict[str, Any] = None) -> Intent:
        """
        Parse user input into an actionable intent.
        
        Args:
            user_input: Natural language input from user
            context: Current Blender context and scene state
            
        Returns:
            Parsed intent with type, confidence, and parameters
        """
        # Normalize input
        normalized_input = self._normalize_input(user_input)
        
        # Analyze context if provided
        scene_context = self.context_analyzer.analyze(context) if context else {}
        
        # Classify intent
        intent_type, confidence = self.intent_classifier.classify(normalized_input)
        
        # Extract parameters
        parameters = self._extract_parameters(normalized_input, intent_type)
        
        # Generate suggestions
        suggestions = self._generate_suggestions(intent_type, parameters, scene_context)
        
        return Intent(
            type=intent_type,
            confidence=confidence,
            parameters=parameters,
            context=scene_context,
            suggestions=suggestions
        )
    
    def _normalize_input(self, user_input: str) -> str:
        """Normalize user input for better parsing."""
        # Convert to lowercase
        normalized = user_input.lower()
        
        # Remove extra whitespace
        normalized = re.sub(r'\s+', ' ', normalized).strip()
        
        # Handle common abbreviations
        abbreviations = {
            'obj': 'object',
            'mat': 'material',
            'tex': 'texture',
            'anim': 'animate',
            'mod': 'modifier',
            'light': 'lighting',
            'render': 'rendering'
        }
        
        for abbrev, full in abbreviations.items():
            normalized = re.sub(r'\b' + abbrev + r'\b', full, normalized)
        
        return normalized
    
    def _load_patterns(self) -> Dict[IntentType, List[str]]:
        """Load intent recognition patterns."""
        return {
            IntentType.CREATE_OBJECT: [
                r'create\s+(?:a\s+)?(\w+)',
                r'make\s+(?:a\s+)?(\w+)',
                r'add\s+(?:a\s+)?(\w+)',
                r'generate\s+(?:a\s+)?(\w+)',
                r'build\s+(?:a\s+)?(\w+)'
            ],
            IntentType.MODIFY_OBJECT: [
                r'modify\s+(?:the\s+)?(\w+)',
                r'change\s+(?:the\s+)?(\w+)',
                r'edit\s+(?:the\s+)?(\w+)',
                r'update\s+(?:the\s+)?(\w+)',
                r'alter\s+(?:the\s+)?(\w+)'
            ],
            IntentType.CREATE_MATERIAL: [
                r'create\s+(?:a\s+)?material\s+(?:that\s+)?(?:looks\s+)?(?:like\s+)?(\w+)',
                r'make\s+(?:a\s+)?material\s+(?:that\s+)?(?:looks\s+)?(?:like\s+)?(\w+)',
                r'add\s+(?:a\s+)?material\s+(?:that\s+)?(?:looks\s+)?(?:like\s+)?(\w+)',
                r'generate\s+(?:a\s+)?material\s+(?:that\s+)?(?:looks\s+)?(?:like\s+)?(\w+)'
            ],
            IntentType.ANIMATE_OBJECT: [
                r'animate\s+(?:the\s+)?(\w+)',
                r'add\s+animation\s+to\s+(?:the\s+)?(\w+)',
                r'make\s+(?:the\s+)?(\w+)\s+move',
                r'create\s+animation\s+for\s+(?:the\s+)?(\w+)'
            ],
            IntentType.OPTIMIZE_SCENE: [
                r'optimize\s+(?:the\s+)?scene',
                r'improve\s+(?:the\s+)?scene',
                r'make\s+(?:the\s+)?scene\s+better',
                r'enhance\s+(?:the\s+)?scene'
            ],
            IntentType.RENDER_SCENE: [
                r'render\s+(?:the\s+)?scene',
                r'create\s+(?:a\s+)?render',
                r'generate\s+(?:a\s+)?render',
                r'make\s+(?:a\s+)?render'
            ]
        }
    
    def _extract_parameters(self, user_input: str, intent_type: IntentType) -> Dict[str, Any]:
        """Extract parameters from user input based on intent type."""
        parameters = {}
        
        if intent_type == IntentType.CREATE_OBJECT:
            # Extract object type and properties
            object_match = re.search(r'create\s+(?:a\s+)?(\w+)(?:\s+with\s+(.+))?', user_input)
            if object_match:
                parameters['object_type'] = object_match.group(1)
                if object_match.group(2):
                    parameters['properties'] = self._parse_properties(object_match.group(2))
        
        elif intent_type == IntentType.CREATE_MATERIAL:
            # Extract material properties
            material_match = re.search(r'looks?\s+like\s+(\w+)', user_input)
            if material_match:
                parameters['material_type'] = material_match.group(1)
            
            # Extract additional properties
            if 'realistic' in user_input:
                parameters['realistic'] = True
            if 'procedural' in user_input:
                parameters['procedural'] = True
        
        elif intent_type == IntentType.ANIMATE_OBJECT:
            # Extract animation properties
            if 'bounce' in user_input:
                parameters['animation_type'] = 'bounce'
            elif 'rotate' in user_input:
                parameters['animation_type'] = 'rotation'
            elif 'scale' in user_input:
                parameters['animation_type'] = 'scale'
            else:
                parameters['animation_type'] = 'custom'
        
        return parameters
    
    def _parse_properties(self, properties_text: str) -> Dict[str, Any]:
        """Parse object properties from text."""
        properties = {}
        
        # Parse size
        size_match = re.search(r'size\s+(\d+)', properties_text)
        if size_match:
            properties['size'] = float(size_match.group(1))
        
        # Parse position
        pos_match = re.search(r'at\s+\(([^)]+)\)', properties_text)
        if pos_match:
            coords = pos_match.group(1).split(',')
            properties['position'] = [float(x.strip()) for x in coords]
        
        # Parse color
        color_match = re.search(r'color\s+(\w+)', properties_text)
        if color_match:
            properties['color'] = color_match.group(1)
        
        return properties
    
    def _generate_suggestions(self, intent_type: IntentType, parameters: Dict[str, Any], context: Dict[str, Any]) -> List[str]:
        """Generate helpful suggestions based on intent and context."""
        suggestions = []
        
        if intent_type == IntentType.CREATE_OBJECT:
            object_type = parameters.get('object_type', '')
            if object_type == 'tree':
                suggestions.extend([
                    "Consider adding a displacement modifier for realistic bark",
                    "Use particle system for leaves",
                    "Add subsurface scattering for realistic wood"
                ])
            elif object_type == 'cube':
                suggestions.extend([
                    "Consider beveling edges for smoother appearance",
                    "Add subdivision surface for organic look",
                    "Use array modifier for repetition"
                ])
        
        elif intent_type == IntentType.CREATE_MATERIAL:
            material_type = parameters.get('material_type', '')
            if material_type == 'metal':
                suggestions.extend([
                    "Add roughness map for realistic metal",
                    "Use environment texture for reflections",
                    "Consider adding scratches or wear"
                ])
            elif material_type == 'wood':
                suggestions.extend([
                    "Add wood grain texture",
                    "Use bump mapping for surface detail",
                    "Consider adding knots and variations"
                ])
        
        return suggestions


class ContextAnalyzer:
    """Analyzes current Blender context and scene state."""
    
    def analyze(self, context: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze current scene context."""
        scene_context = {
            'objects': self._analyze_objects(),
            'materials': self._analyze_materials(),
            'lights': self._analyze_lights(),
            'camera': self._analyze_camera(),
            'render_settings': self._analyze_render_settings(),
            'user_preferences': self._analyze_user_preferences()
        }
        return scene_context
    
    def _analyze_objects(self) -> List[Dict[str, Any]]:
        """Analyze objects in the scene."""
        objects = []
        for obj in bpy.context.scene.objects:
            obj_data = {
                'name': obj.name,
                'type': obj.type,
                'location': list(obj.location),
                'rotation': list(obj.rotation_euler),
                'scale': list(obj.scale),
                'modifiers': [mod.name for mod in obj.modifiers],
                'materials': [mat.name for mat in obj.data.materials] if hasattr(obj.data, 'materials') else []
            }
            objects.append(obj_data)
        return objects
    
    def _analyze_materials(self) -> List[Dict[str, Any]]:
        """Analyze materials in the scene."""
        materials = []
        for mat in bpy.data.materials:
            mat_data = {
                'name': mat.name,
                'use_nodes': mat.use_nodes,
                'node_count': len(mat.node_tree.nodes) if mat.node_tree else 0
            }
            materials.append(mat_data)
        return materials
    
    def _analyze_lights(self) -> List[Dict[str, Any]]:
        """Analyze lights in the scene."""
        lights = []
        for obj in bpy.context.scene.objects:
            if obj.type == 'LIGHT':
                light_data = {
                    'name': obj.name,
                    'type': obj.data.type,
                    'energy': obj.data.energy,
                    'color': list(obj.data.color)
                }
                lights.append(light_data)
        return lights
    
    def _analyze_camera(self) -> Dict[str, Any]:
        """Analyze camera settings."""
        camera = None
        for obj in bpy.context.scene.objects:
            if obj.type == 'CAMERA':
                camera = obj
                break
        
        if camera:
            return {
                'name': camera.name,
                'location': list(camera.location),
                'rotation': list(camera.rotation_euler),
                'fov': camera.data.angle
            }
        return {}
    
    def _analyze_render_settings(self) -> Dict[str, Any]:
        """Analyze render settings."""
        scene = bpy.context.scene
        return {
            'engine': scene.render.engine,
            'resolution_x': scene.render.resolution_x,
            'resolution_y': scene.render.resolution_y,
            'samples': getattr(scene.cycles, 'samples', 128) if scene.render.engine == 'CYCLES' else None
        }
    
    def _analyze_user_preferences(self) -> Dict[str, Any]:
        """Analyze user preferences and recent actions."""
        # This would integrate with a learning system
        return {
            'preferred_workflow': 'modeling',  # Placeholder
            'recent_operations': [],  # Placeholder
            'skill_level': 'intermediate'  # Placeholder
        }


class IntentClassifier:
    """Classifies user input into intent types."""
    
    def __init__(self):
        """Initialize the intent classifier."""
        self.keywords = self._load_keywords()
    
    def classify(self, user_input: str) -> tuple[IntentType, float]:
        """Classify user input into intent type with confidence score."""
        scores = {}
        
        for intent_type, keywords in self.keywords.items():
            score = self._calculate_score(user_input, keywords)
            scores[intent_type] = score
        
        # Find best match
        best_intent = max(scores, key=scores.get)
        confidence = scores[best_intent]
        
        return best_intent, confidence
    
    def _load_keywords(self) -> Dict[IntentType, List[str]]:
        """Load keywords for intent classification."""
        return {
            IntentType.CREATE_OBJECT: ['create', 'make', 'add', 'generate', 'build', 'new'],
            IntentType.MODIFY_OBJECT: ['modify', 'change', 'edit', 'update', 'alter', 'transform'],
            IntentType.CREATE_MATERIAL: ['material', 'texture', 'shader', 'look', 'appearance'],
            IntentType.ANIMATE_OBJECT: ['animate', 'animation', 'move', 'motion', 'keyframe'],
            IntentType.OPTIMIZE_SCENE: ['optimize', 'improve', 'better', 'enhance', 'performance'],
            IntentType.RENDER_SCENE: ['render', 'image', 'picture', 'output', 'final'],
            IntentType.APPLY_MODIFIER: ['modifier', 'subdivision', 'bevel', 'array', 'mirror'],
            IntentType.SETUP_LIGHTING: ['light', 'lighting', 'illuminate', 'bright', 'shadow'],
            IntentType.CREATE_TEXTURE: ['texture', 'pattern', 'surface', 'detail', 'bump'],
            IntentType.HELP_REQUEST: ['help', 'how', 'what', 'why', 'tutorial', 'guide']
        }
    
    def _calculate_score(self, user_input: str, keywords: List[str]) -> float:
        """Calculate confidence score for intent classification."""
        score = 0.0
        input_words = user_input.split()
        
        for keyword in keywords:
            if keyword in user_input:
                score += 1.0
            for word in input_words:
                if keyword in word or word in keyword:
                    score += 0.5
        
        # Normalize score
        return min(score / len(keywords), 1.0)


# Example usage
if __name__ == "__main__":
    parser = IntentParser()
    
    # Test intent parsing
    test_inputs = [
        "Create a realistic tree",
        "Make this material look like weathered metal",
        "Animate this object with a bouncing motion",
        "Optimize this scene for rendering"
    ]
    
    for input_text in test_inputs:
        intent = parser.parse(input_text)
        print(f"Input: {input_text}")
        print(f"Intent: {intent.type.value}")
        print(f"Confidence: {intent.confidence:.2f}")
        print(f"Parameters: {intent.parameters}")
        print(f"Suggestions: {intent.suggestions}")
        print("-" * 50) 