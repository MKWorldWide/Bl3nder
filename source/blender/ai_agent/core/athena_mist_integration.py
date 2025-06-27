"""
AthenaMist Integration for Blender AI Agent
==========================================

This module integrates AthenaMist AI capabilities with Blender, providing
advanced natural language understanding, creative suggestions, and intelligent
workflow automation. Inspired by creative Skyrim mods for immersive AI experiences.
"""

import json
import requests
import asyncio
import aiohttp
import re
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass
from enum import Enum
import bpy
import bmesh
from mathutils import Vector, Matrix


class AthenaMistMode(Enum):
    """AthenaMist interaction modes inspired by creative AI mods."""
    CREATIVE_ASSISTANT = "creative_assistant"  # Like a creative companion
    TECHNICAL_EXPERT = "technical_expert"      # Technical workflow expert
    ARTISTIC_MENTOR = "artistic_mentor"        # Artistic guidance and inspiration
    WORKFLOW_OPTIMIZER = "workflow_optimizer"  # Performance and efficiency
    IMMERSIVE_STORYTELLER = "immersive_storyteller"  # Narrative and context


@dataclass
class AthenaMistResponse:
    """Response from AthenaMist AI."""
    content: str
    confidence: float
    suggestions: List[str]
    actions: List[Dict[str, Any]]
    context: Dict[str, Any]
    mode: AthenaMistMode


class AthenaMistIntegration:
    """
    AthenaMist AI integration for Blender.
    
    Provides advanced AI capabilities including:
    - Natural language understanding
    - Creative suggestions
    - Workflow automation
    - Artistic guidance
    - Immersive storytelling
    """
    
    def __init__(self, api_key: str = None, base_url: str = "https://api.athenamist.ai"):
        """Initialize AthenaMist integration."""
        self.api_key = api_key
        self.base_url = base_url
        self.session = None
        self.current_mode = AthenaMistMode.CREATIVE_ASSISTANT
        self.conversation_history = []
        self.user_preferences = {}
        self.creative_context = {}
        
        # Initialize creative personality inspired by Skyrim mods
        self.personality = self._initialize_personality()
    
    def _initialize_personality(self) -> Dict[str, Any]:
        """Initialize AI personality inspired by creative mods."""
        return {
            'name': 'Athena',
            'role': 'Creative AI Assistant',
            'personality_traits': [
                'creative', 'helpful', 'knowledgeable', 'inspiring',
                'patient', 'enthusiastic', 'artistic', 'technical'
            ],
            'specializations': [
                '3D modeling', 'material creation', 'animation',
                'lighting', 'rendering', 'workflow optimization'
            ],
            'communication_style': 'friendly and professional',
            'creative_approach': 'inspirational and practical'
        }
    
    async def initialize_session(self):
        """Initialize async session for AthenaMist API."""
        if not self.session:
            self.session = aiohttp.ClientSession()
    
    async def close_session(self):
        """Close async session."""
        if self.session:
            await self.session.close()
            self.session = None
    
    async def send_message(self, 
                          message: str, 
                          context: Dict[str, Any] = None,
                          mode: AthenaMistMode = None) -> AthenaMistResponse:
        """
        Send message to AthenaMist and get response.
        
        Args:
            message: User message
            context: Blender scene context
            mode: AthenaMist interaction mode
            
        Returns:
            AthenaMist response with content, suggestions, and actions
        """
        await self.initialize_session()
        
        # Prepare request payload
        payload = self._prepare_payload(message, context, mode)
        
        try:
            # Send request to AthenaMist API
            async with self.session.post(
                f"{self.base_url}/v1/chat/completions",
                headers=self._get_headers(),
                json=payload
            ) as response:
                if response.status == 200:
                    data = await response.json()
                    return self._parse_response(data, mode)
                else:
                    # Fallback to local processing
                    return self._fallback_response(message, context, mode)
        
        except Exception as e:
            print(f"AthenaMist API error: {e}")
            return self._fallback_response(message, context, mode)
    
    def _prepare_payload(self, message: str, context: Dict[str, Any], mode: AthenaMistMode) -> Dict[str, Any]:
        """Prepare payload for AthenaMist API request."""
        # Build system prompt based on mode and context
        system_prompt = self._build_system_prompt(mode, context)
        
        # Build conversation history
        messages = [
            {"role": "system", "content": system_prompt},
            *self.conversation_history[-10:],  # Last 10 messages for context
            {"role": "user", "content": message}
        ]
        
        return {
            "model": "athena-mist-v1",
            "messages": messages,
            "temperature": 0.7,
            "max_tokens": 1000,
            "stream": False
        }
    
    def _build_system_prompt(self, mode: AthenaMistMode, context: Dict[str, Any]) -> str:
        """Build system prompt based on mode and context."""
        base_prompt = f"""
You are Athena, an intelligent AI assistant integrated with Blender 3D software.
Your personality: {self.personality['communication_style']}
Your role: {self.personality['role']}
Your specializations: {', '.join(self.personality['specializations'])}

Current Blender context:
- Objects in scene: {len(context.get('objects', []))}
- Materials: {len(context.get('materials', []))}
- Render engine: {context.get('render_settings', {}).get('engine', 'Unknown')}
- User mode: {context.get('user_mode', 'Object Mode')}

Mode: {mode.value if mode else self.current_mode.value}

Provide helpful, creative, and practical assistance for 3D creation in Blender.
Always include specific Blender operations and suggestions.
Be inspiring and encouraging while maintaining technical accuracy.
"""
        
        # Add mode-specific instructions
        if mode == AthenaMistMode.CREATIVE_ASSISTANT:
            base_prompt += "\nFocus on creative inspiration and artistic guidance."
        elif mode == AthenaMistMode.TECHNICAL_EXPERT:
            base_prompt += "\nFocus on technical accuracy and workflow optimization."
        elif mode == AthenaMistMode.ARTISTIC_MENTOR:
            base_prompt += "\nFocus on artistic development and creative techniques."
        elif mode == AthenaMistMode.WORKFLOW_OPTIMIZER:
            base_prompt += "\nFocus on efficiency and performance optimization."
        elif mode == AthenaMistMode.IMMERSIVE_STORYTELLER:
            base_prompt += "\nFocus on narrative context and immersive experiences."
        
        return base_prompt
    
    def _get_headers(self) -> Dict[str, str]:
        """Get headers for API request."""
        headers = {
            "Content-Type": "application/json",
            "User-Agent": "Blender-AI-Agent/1.0"
        }
        if self.api_key:
            headers["Authorization"] = f"Bearer {self.api_key}"
        return headers
    
    def _parse_response(self, data: Dict[str, Any], mode: AthenaMistMode) -> AthenaMistResponse:
        """Parse AthenaMist API response."""
        try:
            content = data.get('choices', [{}])[0].get('message', {}).get('content', '')
            
            # Extract structured data from response
            suggestions = self._extract_suggestions(content)
            actions = self._extract_actions(content)
            context = self._extract_context(content)
            
            # Update conversation history
            self.conversation_history.append({"role": "assistant", "content": content})
            
            return AthenaMistResponse(
                content=content,
                confidence=0.9,  # High confidence for API responses
                suggestions=suggestions,
                actions=actions,
                context=context,
                mode=mode or self.current_mode
            )
        
        except Exception as e:
            print(f"Error parsing AthenaMist response: {e}")
            return self._fallback_response("", {}, mode)
    
    def _extract_suggestions(self, content: str) -> List[str]:
        """Extract suggestions from AthenaMist response."""
        suggestions = []
        
        # Look for suggestion patterns
        lines = content.split('\n')
        for line in lines:
            if any(keyword in line.lower() for keyword in ['suggest', 'try', 'consider', 'recommend']):
                suggestions.append(line.strip())
        
        return suggestions[:5]  # Limit to 5 suggestions
    
    def _extract_actions(self, content: str) -> List[Dict[str, Any]]:
        """Extract actionable items from AthenaMist response."""
        actions = []
        
        # Parse action patterns
        action_patterns = [
            r'create\s+(\w+)',
            r'add\s+(\w+)',
            r'modify\s+(\w+)',
            r'animate\s+(\w+)',
            r'render\s+(\w+)'
        ]
        
        for pattern in action_patterns:
            matches = re.findall(pattern, content.lower())
            for match in matches:
                actions.append({
                    'type': 'suggestion',
                    'action': match,
                    'description': f"Action: {match}"
                })
        
        return actions
    
    def _extract_context(self, content: str) -> Dict[str, Any]:
        """Extract context information from AthenaMist response."""
        return {
            'mood': self._detect_mood(content),
            'complexity': self._assess_complexity(content),
            'focus_area': self._identify_focus_area(content)
        }
    
    def _detect_mood(self, content: str) -> str:
        """Detect the mood/tone of the response."""
        positive_words = ['great', 'excellent', 'wonderful', 'amazing', 'inspiring']
        technical_words = ['technical', 'precise', 'accurate', 'efficient']
        creative_words = ['creative', 'artistic', 'beautiful', 'inspiring', 'imaginative']
        
        content_lower = content.lower()
        
        if any(word in content_lower for word in positive_words):
            return 'positive'
        elif any(word in content_lower for word in technical_words):
            return 'technical'
        elif any(word in content_lower for word in creative_words):
            return 'creative'
        else:
            return 'neutral'
    
    def _assess_complexity(self, content: str) -> str:
        """Assess the complexity level of the response."""
        word_count = len(content.split())
        technical_terms = ['modifier', 'shader', 'texture', 'animation', 'rendering']
        
        technical_count = sum(1 for term in technical_terms if term in content.lower())
        
        if technical_count > 3 or word_count > 200:
            return 'advanced'
        elif technical_count > 1 or word_count > 100:
            return 'intermediate'
        else:
            return 'beginner'
    
    def _identify_focus_area(self, content: str) -> str:
        """Identify the main focus area of the response."""
        focus_areas = {
            'modeling': ['mesh', 'geometry', 'vertex', 'face', 'edge', 'model'],
            'materials': ['material', 'texture', 'shader', 'surface', 'appearance'],
            'animation': ['animate', 'keyframe', 'motion', 'timeline', 'action'],
            'lighting': ['light', 'illumination', 'shadow', 'brightness', 'exposure'],
            'rendering': ['render', 'output', 'image', 'quality', 'settings']
        }
        
        content_lower = content.lower()
        scores = {}
        
        for area, keywords in focus_areas.items():
            scores[area] = sum(1 for keyword in keywords if keyword in content_lower)
        
        if scores:
            return max(scores, key=scores.get)
        else:
            return 'general'
    
    def _fallback_response(self, message: str, context: Dict[str, Any], mode: AthenaMistMode) -> AthenaMistResponse:
        """Generate fallback response when API is unavailable."""
        fallback_content = self._generate_fallback_content(message, context, mode)
        
        return AthenaMistResponse(
            content=fallback_content,
            confidence=0.6,  # Lower confidence for fallback
            suggestions=self._generate_fallback_suggestions(context),
            actions=[],
            context={'mood': 'helpful', 'complexity': 'intermediate', 'focus_area': 'general'},
            mode=mode or self.current_mode
        )
    
    def _generate_fallback_content(self, message: str, context: Dict[str, Any], mode: AthenaMistMode) -> str:
        """Generate fallback content when API is unavailable."""
        if 'create' in message.lower():
            return "I'd be happy to help you create something in Blender! What type of object are you thinking of? I can guide you through the process step by step."
        elif 'material' in message.lower():
            return "Materials are essential for bringing your 3D objects to life! Would you like to create a realistic material, or are you looking for something more stylized?"
        elif 'animate' in message.lower():
            return "Animation brings movement and life to your creations! I can help you set up keyframes, create smooth motion, or add special effects."
        else:
            return "I'm here to help with your 3D creation journey! Whether you need help with modeling, materials, animation, or rendering, I'm ready to assist."
    
    def _generate_fallback_suggestions(self, context: Dict[str, Any]) -> List[str]:
        """Generate fallback suggestions based on context."""
        suggestions = []
        
        if len(context.get('objects', [])) == 0:
            suggestions.append("Try creating a basic cube to get started with 3D modeling")
            suggestions.append("Consider adding some lighting to your scene")
        
        if len(context.get('materials', [])) < 2:
            suggestions.append("Create a new material to add visual interest to your objects")
            suggestions.append("Experiment with different material types like Principled BSDF")
        
        suggestions.append("Use the subdivision surface modifier for smoother geometry")
        suggestions.append("Try different viewport shading modes to see your work in different ways")
        
        return suggestions[:3]
    
    def update_user_preferences(self, preferences: Dict[str, Any]):
        """Update user preferences for personalized responses."""
        self.user_preferences.update(preferences)
    
    def set_mode(self, mode: AthenaMistMode):
        """Set AthenaMist interaction mode."""
        self.current_mode = mode
    
    def get_conversation_summary(self) -> Dict[str, Any]:
        """Get summary of conversation history."""
        return {
            'total_messages': len(self.conversation_history),
            'current_mode': self.current_mode.value,
            'user_preferences': self.user_preferences,
            'recent_topics': self._extract_recent_topics()
        }
    
    def _extract_recent_topics(self) -> List[str]:
        """Extract recent conversation topics."""
        topics = []
        recent_messages = self.conversation_history[-5:]  # Last 5 messages
        
        for message in recent_messages:
            content = message.get('content', '').lower()
            if 'create' in content:
                topics.append('object_creation')
            elif 'material' in content:
                topics.append('materials')
            elif 'animate' in content:
                topics.append('animation')
            elif 'render' in content:
                topics.append('rendering')
        
        return list(set(topics))  # Remove duplicates


# Example usage
if __name__ == "__main__":
    # Initialize AthenaMist integration
    athena = AthenaMistIntegration()
    
    # Example conversation
    async def test_conversation():
        context = {
            'objects': [{'name': 'Cube', 'type': 'MESH'}],
            'materials': [{'name': 'Material'}],
            'render_settings': {'engine': 'CYCLES'},
            'user_mode': 'Object Mode'
        }
        
        response = await athena.send_message(
            "I want to create a realistic tree",
            context=context,
            mode=AthenaMistMode.CREATIVE_ASSISTANT
        )
        
        print(f"Athena: {response.content}")
        print(f"Suggestions: {response.suggestions}")
        print(f"Actions: {response.actions}")
    
    # Run test
    asyncio.run(test_conversation()) 