<?php
    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;
 class Importer_Airtel extends CI_Controller {

        public function __construct(){
            parent::__construct();

        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/airtel-form");
        }

        public function importer() {

            // $upload_file_airtel= $_FILES["airtel"]["name"];
            // $upload_file_igor= $_FILES["airtel"]["name"];
            // $extension_airtel = pathinfo($upload_file_airtel, PATHINFO_EXTENSION);
            // $extension_igor = pathinfo($upload_file_igor, PATHINFO_EXTENSION);

            $airtel_read = $this->readData($_FILES["airtel"]["name"], $_FILES["airtel"]["tmp_name"]);

            $igor_read= $this->readData($_FILES["igor"]["name"], $_FILES["igor"]["tmp_name"]);

            $airtel= array();
            $igor= array();
            if (count($airtel_read) > 1 ) {
                
                for($i=2; $i < count($airtel_read); $i++) {

                    // $date= $airtel_read[$i][2];
                    // $date = DateTime::createFromFormat('d-M-Y', $date);
                    // $date = $date->format('d/m/Y');
                    $airtel[]= array(
                       "TRANSFER_ID" => $airtel_read[$i][0],
                       "transfer_date" =>  $airtel_read[$i][1],
                       "external_id" => $airtel_read[$i][2],
                       "account_no" => $airtel_read[$i][3],
                       "sender_msisdn" => $airtel_read[$i][4],
                       "dest_msisdn" => $airtel_read[$i][5],
                       "amount" => $airtel_read[$i][6],
                       "description" => $airtel_read[$i][7],
                       "service_name" => $airtel_read[$i][8],
                       "reference_number" => $airtel_read[$i][9]
                    );
                }
            }

            if (count($igor_read) > 1 ) {
                
                for($i=2; $i < count($igor_read); $i++) {

                    // $date= $airtel_read[$i][2];
                    // $date = DateTime::createFromFormat('d-M-Y', $date);
                    // $date = $date->format('d/m/Y');
                    $igor[]= array(
                       "COMPTE" => $igor_read[$i][0],
                       "DATE_OPER" =>  $igor_read[$i][1],
                       "DATE_VAL" => $igor_read[$i][2],
                       "DEVISE" => $igor_read[$i][3],
                       "MONTANT" => $igor_read[$i][4],
                       "LIBELLE" => $igor_read[$i][5],
                       "OPER" => $igor_read[$i][6],
                       "EXPL" => $igor_read[$i][7],
                       "REF_IGOR" => $igor_read[$i][8],
                    );
                }
            }

            $airtel = $this->supprimerEspace($airtel);
            $igor = $this->supprimerEspace($airtel);

            $deallocation = $this->filtrerDeallocation($airtel);

            echo "<pre>";
            echo "--------------------------------------------------";
                print_r($airtel);
            echo "--------------------------------------------------";
            echo "</pre>";


            // echo "<pre>";
            // echo "--------------------------------------------------";
            //     print_r($igor);
            // echo "--------------------------------------------------";
            // echo "</pre>";

            // if($extension_airtel ==="csv") {
            //     $reader_airtel= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            // } else if ($extension_airtel =="xls") {
            //     $reader_airtel= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            // } else {
            //     $reader_airtel=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            // }

            // if($extension_igor ==="csv") {
            //     $reader_igor= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            // } else if ($extension_igor =="xls") {
            //     $reader_igor= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            // } else {
            //     $reader_igor=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            // }

            // $spreadsheet= $reader_airtel->load($_FILES["airtel"]["tmp_name"]);
            // $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            // // $spreadsheet= $reader->load($_FILES["airtel"]["tmp_name"]);
            // // $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            // // $spreadsheet= $reader->load($_FILES["airtel"]["tmp_name"]);
            // // $sheetdata= $spreadsheet->getActiveSheet()->toArray();
            // echo "<pre>";
            //     print_r($sheetdata);
            // echo "</pre>";



        }

        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension_airtel =="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

           

        }

        public function supprimerEspace($data) {
            $resultat= array();
            foreach($data as $key=> $valeur) {
                $resultat[$key] = str_replace(" ", "", $valeur);
            }

            return $resultat;
        }

        public function filtrerDeallocation ($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["service_name"] ==="DeallocationTransfer") {
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerCashIn($data) {
            $reultat= array();

            foreach($data as $item) {
                if(!empty($item["external_id"]) && ($item["service_name"]==="BanktoWalletTransfer" || $item["service_name"] === "ChannelBanktoWalletTransfer" )) {
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerRollback($data) {
            $resultat= array();
        }
 }



?>