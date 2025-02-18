import sys

def load_acronyms(file_path):
    """Reads the Markdown file and loads its content into a structured dictionary."""
    sections = {}
    current_section = None
    with open(file_path, 'r', encoding='utf-8') as f:
        for line in f:
            line = line.strip()
            if line.startswith("## "):  # Detect section headers
                current_section = line[3:]  # Extract letter (A, B, C, etc.)
                sections.setdefault(current_section, [])
            elif current_section and line:  # Add acronym to its section
                sections[current_section].append(line)
    return sections

def save_acronyms(file_path, sections):
    """Writes the updated dictionary back to the Markdown file, keeping sorted acronyms."""
    with open(file_path, 'w', encoding='utf-8') as f:
        for section in sorted(sections.keys()):  # Sort sections (A-Z)
            f.write(f"## {section}\n")
            for acronym in sorted(sections[section], key=str.lower):  # Sort acronyms
                f.write(f"{acronym}  \n")
            f.write("\n")  # Blank line for spacing

def add_acronym(file_path, acronym, definition=""):
    """Adds a new acronym with an optional definition to the Markdown file."""
    sections = load_acronyms(file_path)
    section_key = acronym[0].upper()  # Determine the correct section (A, B, C, etc.)
    
    # Create a formatted entry
    entry = f"{acronym} - {definition}" if definition else acronym

    # Ensure the section exists
    if section_key not in sections:
        sections[section_key] = []
    
    # Avoid duplicates before adding
    if entry not in sections[section_key]:
        sections[section_key].append(entry)
        save_acronyms(file_path, sections)
        print(f"✅ Added: {entry}")
    else:
        print(f"⚠️ Acronym '{acronym}' already exists.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python add_acronym.py <file> <acronym> [definition]")
        sys.exit(1)

    file_path = sys.argv[1]
    acronym = sys.argv[2]
    definition = " ".join(sys.argv[3:]) if len(sys.argv) > 3 else ""

    add_acronym(file_path, acronym, definition)

