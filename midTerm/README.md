#**Resumen del problema**
Password decryption
• Decrypt passwords encrypted using crypt (DES)
• Consider 8 characters lengths in the set [a-zA-Z0-9./]
• Library funds may be deprecated or not available. E.g.
CUDA does not provide the crypt C-library function. Must
use a special implementation (check Moodle).
• It’s a (slowed down) search problem.
• Can choose to attack a specific password in a list (in
different positions), or decrypt a full list of passwords (in
this case reduce the search space)
• Can consider only dates or common 8-chars passwords
