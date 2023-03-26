import patoolib
import patoolib.programs
import patoolib.programs.rar
import patoolib.programs.unrar
import patoolib.programs.p7zip
import sys

import tempfile
import glob
import pathlib
import shutil



if __name__ == "__main__": 
    
    paramcount = len(sys.argv) - 1
    
    if (paramcount == 1) : 
        
        temp_dir = tempfile.TemporaryDirectory()
        patoolib.extract_archive(archive = sys.argv[1], verbosity=-1, outdir=temp_dir.name, interactive=False)
        for f in glob.glob(temp_dir.name+'/**/*.bin', recursive=True):
            newPath = str(pathlib.Path(sys.argv[1]).parent.resolve())+'/bios.bin'
            shutil.copy(f, newPath)
            break
           
    if (paramcount == 2) :   
        patoolib.repack_archive(sys.argv[1], sys.argv[2], -1, False)