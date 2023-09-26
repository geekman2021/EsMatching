<?php
require 'vendor/autoload.php';
use PhpOffice\PhpSpreadsheet\IOFactory;
use PhpOffice\PhpSpreadsheet\Spreadsheet;

class UpdateFileExcelExist extends CI_Controller {
    public function __construct() {
        parent::__construct();
        $this->load->database('db1');
    }

    public function index() {
        $path = "C:\Users\Dad's\Desktop\Rappro\Rappro.xlsx";
        
        if (file_exists($path)) {
            $spreadsheet = IOFactory::load($path);
            $worksheet = $spreadsheet->getActiveSheet();
            $lastRow = $worksheet->getHighestDataRow() + 1;

            $donnees = [
                ['Nom', 'Prénom', 'Âge'],
                ['Doe', 'John', 30],
                ['Smith', 'Jane', 28],
            ];

            
            foreach ($donnees as $row) {
                $worksheet->fromArray($row, null, 'A' . $lastRow);
                $lastRow++;
            }


            $writer = new Xlsx($spreadsheet);
            $writer->save($path);

            echo "Données ajoutées avec succès.";
        } else {
            echo "Le fichier n'existe pas.";
        }
    }
}
?>
