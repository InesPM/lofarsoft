import os
import types

"""Autogenerate module documentation in *modules* directory for all modules listed in *modules.lst*"""

def autogen(list,output_path):
    """
    Automatically generates module documentation files for modules in list file.
    *list* filename of module list.
    *output_path* output module documentation goes in this directory.
    """

    print 'Autogenerating module documentation for modules in',list

    # Read modules from list file
    with open(list) as f:
        modules=[m.strip() for m in f.readlines() if m.strip()[0]!='#']

    for m in modules:
        # Try importing the module this is needed for sphinx to function correctly
        try:
            exec('import %s' % m)
        except Exception as e:
            print 'Module',m,'cannot be imported no documentation will be generated.'
            print 'Error:', e
            continue

        # Check if documentation file exists
        if os.path.isfile(output_path.rstrip('/')+'/'+m+'.rst'):
            print 'Module documentation for',m,'exists, skipping.'
            continue

        # Write empty configuration file
        with open(output_path.rstrip('/')+'/'+m+'.rst','w') as f:
            f.write('*'*len(m)+'\n')
            f.write(m+'\n')
            f.write('*'*len(m)+'\n')
            f.write('\n')

            # Extract members of modules
            exec("module_members=dir("+m+")")

            # Autosummary
            f.write('.. currentmodule:: '+m+'\n\n')
            f.write('.. autosummary::\n\n')
            for module_member in module_members:
                if (isinstance(eval(m+"."+module_member), (types.ClassType, types.TypeType, types.FunctionType))
                    and(module_member[0]!='_')):
                    f.write('   '+module_member+'\n')
            f.write('\n\n')

            # Auto module
            f.write('.. automodule:: '+m+'\n')
            f.write('   :members: \n')
            # f.write('   :undoc-members: \n')
            # f.write('   :show-inheritance:\n')
            f.write('\n')

