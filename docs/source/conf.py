# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'BigOS'
copyright = '2026, Operacja System'
author = 'Operacja System'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'breathe',
]

templates_path = ['_templates']
exclude_patterns = []

breathe_projects = {
    "BigOS": "../xml"
}
breathe_default_project = "BigOS"


# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
html_static_path = ['_static']

html_context = {
    "display_github": True,
    "github_user": "YOUR_GITHUB_USERNAME",
    "github_repo": "YOUR_REPO",
    "github_version": "main/docs/",
}
