#!/usr/bin/env python3
"""
AthenaMist AI Agent Demo
========================

This script demonstrates the capabilities of the AthenaMist AI Agent
for Blender. It showcases various AI modes, conversation examples,
and integration features.
"""

import asyncio
import json
import sys
from pathlib import Path

# Add the ai_agent directory to Python path
current_dir = Path(__file__).parent
if str(current_dir) not in sys.path:
    sys.path.append(str(current_dir))

from core.athena_mist_integration import AthenaMistIntegration, AthenaMistMode
from core.intent_parser import IntentParser


class AthenaMistDemo:
    """Demo class for showcasing AthenaMist AI Agent capabilities."""
    
    def __init__(self):
        """Initialize the demo."""
        self.athena = AthenaMistIntegration()
        self.intent_parser = IntentParser()
        self.demo_scenes = self._create_demo_scenes()
    
    def _create_demo_scenes(self):
        """Create demo scene contexts."""
        return {
            'empty_scene': {
                'objects': [],
                'materials': [],
                'render_settings': {'engine': 'CYCLES'},
                'user_mode': 'Object Mode'
            },
            'basic_scene': {
                'objects': [
                    {'name': 'Cube', 'type': 'MESH', 'location': [0, 0, 0]},
                    {'name': 'Light', 'type': 'LIGHT', 'location': [4, 1, 6]}
                ],
                'materials': [
                    {'name': 'Material', 'use_nodes': True}
                ],
                'render_settings': {'engine': 'CYCLES'},
                'user_mode': 'Object Mode'
            },
            'complex_scene': {
                'objects': [
                    {'name': 'Character', 'type': 'MESH', 'location': [0, 0, 0]},
                    {'name': 'Ground', 'type': 'MESH', 'location': [0, -2, 0]},
                    {'name': 'Tree1', 'type': 'MESH', 'location': [3, 0, 0]},
                    {'name': 'Tree2', 'type': 'MESH', 'location': [-3, 0, 0]},
                    {'name': 'Sun', 'type': 'LIGHT', 'location': [5, 5, 10]}
                ],
                'materials': [
                    {'name': 'Skin', 'use_nodes': True},
                    {'name': 'Cloth', 'use_nodes': True},
                    {'name': 'Ground', 'use_nodes': True},
                    {'name': 'Bark', 'use_nodes': True},
                    {'name': 'Leaves', 'use_nodes': True}
                ],
                'render_settings': {'engine': 'CYCLES'},
                'user_mode': 'Object Mode'
            }
        }
    
    async def demo_creative_assistant(self):
        """Demo the Creative Assistant mode."""
        print("\n" + "="*60)
        print("🎨 CREATIVE ASSISTANT MODE DEMO")
        print("="*60)
        
        self.athena.set_mode(AthenaMistMode.CREATIVE_ASSISTANT)
        
        # Demo conversations
        conversations = [
            ("I want to create a magical forest scene", 'empty_scene'),
            ("Make this character look more heroic", 'complex_scene'),
            ("I need inspiration for a sci-fi environment", 'basic_scene'),
            ("How can I make this scene more atmospheric?", 'complex_scene')
        ]
        
        for message, scene_name in conversations:
            print(f"\n👤 User: {message}")
            
            context = self.demo_scenes[scene_name]
            response = await self.athena.send_message(message, context)
            
            print(f"🤖 Athena: {response.content}")
            print(f"   Confidence: {response.confidence:.2f}")
            print(f"   Mode: {response.mode.value}")
            
            if response.suggestions:
                print(f"   💡 Suggestions: {response.suggestions[:2]}")
            
            await asyncio.sleep(1)  # Pause for readability
    
    async def demo_technical_expert(self):
        """Demo the Technical Expert mode."""
        print("\n" + "="*60)
        print("🔧 TECHNICAL EXPERT MODE DEMO")
        print("="*60)
        
        self.athena.set_mode(AthenaMistMode.TECHNICAL_EXPERT)
        
        conversations = [
            ("How do I optimize this scene for rendering?", 'complex_scene'),
            ("What's the best way to rig this character?", 'complex_scene'),
            ("How can I reduce render time?", 'complex_scene'),
            ("What's the most efficient way to create procedural materials?", 'basic_scene')
        ]
        
        for message, scene_name in conversations:
            print(f"\n👤 User: {message}")
            
            context = self.demo_scenes[scene_name]
            response = await self.athena.send_message(message, context)
            
            print(f"🤖 Athena: {response.content}")
            print(f"   Confidence: {response.confidence:.2f}")
            print(f"   Focus Area: {response.context.get('focus_area', 'general')}")
            
            if response.actions:
                print(f"   ⚡ Actions: {[a.get('action', '') for a in response.actions[:2]]}")
            
            await asyncio.sleep(1)
    
    async def demo_artistic_mentor(self):
        """Demo the Artistic Mentor mode."""
        print("\n" + "="*60)
        print("🎭 ARTISTIC MENTOR MODE DEMO")
        print("="*60)
        
        self.athena.set_mode(AthenaMistMode.ARTISTIC_MENTOR)
        
        conversations = [
            ("I'm struggling with composition", 'complex_scene'),
            ("How can I improve the lighting?", 'complex_scene'),
            ("What makes a good character design?", 'complex_scene'),
            ("How do I create mood in my scene?", 'basic_scene')
        ]
        
        for message, scene_name in conversations:
            print(f"\n👤 User: {message}")
            
            context = self.demo_scenes[scene_name]
            response = await self.athena.send_message(message, context)
            
            print(f"🤖 Athena: {response.content}")
            print(f"   Mood: {response.context.get('mood', 'neutral')}")
            print(f"   Complexity: {response.context.get('complexity', 'intermediate')}")
            
            await asyncio.sleep(1)
    
    async def demo_workflow_optimizer(self):
        """Demo the Workflow Optimizer mode."""
        print("\n" + "="*60)
        print("⚡ WORKFLOW OPTIMIZER MODE DEMO")
        print("="*60)
        
        self.athena.set_mode(AthenaMistMode.WORKFLOW_OPTIMIZER)
        
        conversations = [
            ("How can I speed up my workflow?", 'complex_scene'),
            ("What shortcuts should I learn?", 'basic_scene'),
            ("How do I organize my project better?", 'complex_scene'),
            ("What's the most efficient way to texture this?", 'complex_scene')
        ]
        
        for message, scene_name in conversations:
            print(f"\n👤 User: {message}")
            
            context = self.demo_scenes[scene_name]
            response = await self.athena.send_message(message, context)
            
            print(f"🤖 Athena: {response.content}")
            print(f"   Focus Area: {response.context.get('focus_area', 'general')}")
            
            await asyncio.sleep(1)
    
    async def demo_immersive_storyteller(self):
        """Demo the Immersive Storyteller mode."""
        print("\n" + "="*60)
        print("📖 IMMERSIVE STORYTELLER MODE DEMO")
        print("="*60)
        
        self.athena.set_mode(AthenaMistMode.IMMERSIVE_STORYTELLER)
        
        conversations = [
            ("Tell me a story about this character", 'complex_scene'),
            ("What's the history of this place?", 'complex_scene'),
            ("Create a narrative for this scene", 'basic_scene'),
            ("What emotions should this scene convey?", 'complex_scene')
        ]
        
        for message, scene_name in conversations:
            print(f"\n👤 User: {message}")
            
            context = self.demo_scenes[scene_name]
            response = await self.athena.send_message(message, context)
            
            print(f"🤖 Athena: {response.content}")
            print(f"   Mood: {response.context.get('mood', 'neutral')}")
            
            await asyncio.sleep(1)
    
    async def demo_intent_parsing(self):
        """Demo the intent parsing capabilities."""
        print("\n" + "="*60)
        print("🧠 INTENT PARSING DEMO")
        print("="*60)
        
        test_phrases = [
            "Create a cube",
            "Add a metallic material to the sphere",
            "Animate the character walking",
            "Render the scene with high quality",
            "Add lighting to make it more dramatic",
            "I want to model a tree",
            "How do I make this material look realistic?",
            "Set up a camera for this scene"
        ]
        
        for phrase in test_phrases:
            print(f"\n📝 Input: {phrase}")
            
            intent = self.intent_parser.parse_intent(phrase)
            
            print(f"🎯 Intent: {intent.intent_type}")
            print(f"   Confidence: {intent.confidence:.2f}")
            print(f"   Parameters: {intent.parameters}")
            print(f"   Actions: {intent.suggested_actions}")
    
    async def demo_conversation_flow(self):
        """Demo a natural conversation flow."""
        print("\n" + "="*60)
        print("💬 NATURAL CONVERSATION FLOW DEMO")
        print("="*60)
        
        # Simulate a natural conversation
        conversation = [
            ("Hi Athena! I'm new to Blender", 'empty_scene'),
            ("I want to create a simple scene", 'empty_scene'),
            ("Can you help me add some lighting?", 'basic_scene'),
            ("Now I want to make it look more realistic", 'basic_scene'),
            ("What if I want to animate something?", 'basic_scene'),
            ("Thanks for your help!", 'basic_scene')
        ]
        
        for message, scene_name in conversation:
            print(f"\n👤 User: {message}")
            
            context = self.demo_scenes[scene_name]
            response = await self.athena.send_message(message, context)
            
            print(f"🤖 Athena: {response.content}")
            
            await asyncio.sleep(1)
    
    async def run_full_demo(self):
        """Run the complete demo."""
        print("🚀 ATHENAMIST AI AGENT DEMO")
        print("="*60)
        print("Welcome to the AthenaMist AI Agent demonstration!")
        print("This demo showcases the various AI modes and capabilities.")
        print("="*60)
        
        # Run all demo sections
        await self.demo_creative_assistant()
        await self.demo_technical_expert()
        await self.demo_artistic_mentor()
        await self.demo_workflow_optimizer()
        await self.demo_immersive_storyteller()
        await self.demo_intent_parsing()
        await self.demo_conversation_flow()
        
        # Show conversation summary
        summary = self.athena.get_conversation_summary()
        print("\n" + "="*60)
        print("📊 CONVERSATION SUMMARY")
        print("="*60)
        print(f"Total Messages: {summary['total_messages']}")
        print(f"Current Mode: {summary['current_mode']}")
        print(f"Recent Topics: {', '.join(summary['recent_topics'])}")
        
        print("\n🎉 Demo completed! AthenaMist AI Agent is ready to assist you in Blender!")
        print("="*60)


def main():
    """Main demo function."""
    demo = AthenaMistDemo()
    
    try:
        # Run the demo
        asyncio.run(demo.run_full_demo())
    except KeyboardInterrupt:
        print("\n\n⏹️ Demo interrupted by user.")
    except Exception as e:
        print(f"\n❌ Demo error: {e}")
        return 1
    
    return 0


if __name__ == "__main__":
    exit(main()) 