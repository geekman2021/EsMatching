<?php if($_SESSION["isLogin"] !==true) {
  redirect("auth");
}?>

<form id="addform" method="post"  action="<?php echo site_url("Importer_Airtel/importer"); ?>" enctype="multipart/form-data" >
    <div class="container mt-5">
        <div class="row">
            <div class="form-group col-lg-6">
                <label for="airtel">Airtel</label>
                <input type="file" class="form-control" id="input1" name="airtel" accept=".csv, .xls, .xlsx" onchange="checkFileExtension('input1')" required >
                <p id="warning1" class="warning-message"></p>
            </div>
            <div class="form-group col-lg-6">
                <label for="igor">IGOR</label>
                <input type="file" class="form-control" id="input2" name="igor" onchange="checkFileExtension('input2')" accept=".csv, .xls, .xlsx" required>
                <p id="warning2" class="warning-message"></p>
            </div>
        </div>
        <div class="container">
            <div class="text-center">
                <button type="reset" class="btn btn-warning" id="btn-reinitialiser">Reinitialiser</button>
                <button type="submit" class="btn btn-success" id="btn-ajouter">Comparer</button>
            </div>
        </div>
    </div>
</form>

<!-- <h4 class="spinner">
Loading...
</h4> -->

<!-- <div class="spinner-border" role="status">
  <span class="sr-only">Loading...</span>
</div> -->


<!-- <div class="modal fade" style="margin-left: auto; margin-right: auto; margin-left: 100px;" id="progressModal" tabindex="-1" role="dialog" aria-labelledby="progressModalLabel" aria-hidden="true">
  <div class="modal-dialog modal-dialog-centered" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title" id="progressModalLabel">Progression de l'importation</h5>
        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
          <span aria-hidden="true">&times;</span>
        </button>
      </div>
      <div class="modal-body">
        <div class="progress"> -->
          <!-- <div id="progress-bar" class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">
          Traitement en cours ...
          </div>
        <button class="btn btn-primary" type="button" disabled>
            <span class="spinner-border spinner-border-sm"  style="animation: spin 2s linear infinite;" role="status" aria-hidden="true"></span>
            Traitement en cours ...
        </button>
        </div>
      </div>
    </div>
  </div>
</div> -->

<!-- <div class="container mt-5"> --> 
    <!-- <h1>Progression en Temps Réel</h1>
     <div class="progress">
        <div class="progress-bar" id="progress-bar" role="progressbar" style="width: 0%;" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">0%</div>
    </div>
</div> -->
 <!-- <div class="container mt-5">
    <div class="modal fade" id="progressModal" tabindex="-1" role="dialog" aria-labelledby="progressModalLabel" aria-hidden="true">
        <div class="modal-dialog modal-dialog-centered" role="document p-3">
            <div class="modal-content">
                <div class="modal-header">
                     <h5 class="modal-title text-center" id="progressModalLabel">Progression de l'importation</h5>
                        <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                            <span aria-hidden="true">&times;</span>
                        </button> 
                </div>
                <div class="modal-body">
                    <div class="progress" >
                        <div class="progress-bar" id="progress-bar" role="progressbar" style="width: 0%;" aria-valuenow="0" aria-valuemin="0" aria-valuemax="100">0%</div>
                    </div>
                    <div>
                        
                    </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>  -->

<script>

function checkFileExtension(inputId) {
    var input = document.getElementById(inputId);
    var file = input.files[0];

    var fileName = file.name;
    var fileExtension = fileName.split('.').pop().toLowerCase();
    var warningMessage = document.getElementById('warning' + inputId.charAt(inputId.length - 1)); // Obtenez l'élément de message d'avertissement correspondant

    if (fileExtension === 'xls' || fileExtension === 'csv' || fileExtension=== 'xlsx' ) {
        warningMessage.textContent = '';
        warningMessage.style.display = 'none';
        document.getElementById("btn-ajouter").disabled= false;
    } else {
        warningMessage.textContent = 'Le fichier doit avoir une extension .xls';
        let element= $(".warning-message");

        element.css({
            'color': 'red',
            'font-weight': 'bold',
            'margin-top': '10px',
            'display': 'none'
        });
        warningMessage.style.display = 'block';
        input.value = '';
        document.getElementById("btn-ajouter").disabled= true;
    }

}


function updateProgressBar() {
    const progressBar = document.getElementById('progress-bar');
    let progressValue = 0;

    const interval = setInterval(() => {
        if (progressValue >= 100) {
            clearInterval(interval);
            progressBar.style.width = '100%';
            progressBar.innerText = '100%';
            $('#progressModal').modal('hide');
            swal({
                    title: "Succès",
                    text: "Données importées avec succès",
                    icon: "success",
                    button: "OK",
                }); 
        } else {
            progressValue += 1;
            progressBar.style.width = `${progressValue}%`;
            progressBar.innerText = `${progressValue}%`;
        }
    }, 100);
}

// function showSpinner() {  
//     $('.spinner').show();
// }

// $(document).ready(function() {
//     $('.spinner').hide();
// });
    
// $(document).ready(function() {
//     $('#progressModal').modal('show');
//     updateProgressBar();
// });

// function showSpinner() {
//     $('#spinner').show();
// }

// function hideSpinner() {
//     $('#spinner').hide();
// }

// $(document).ready(function() {
//     $('.spinner').hide();
//     $('#addform').submit(function(event) {
//         event.preventDefault();

//         var formData = new FormData(this);
//         $('.spinner').show();
    
//         $.ajax({
//             url: '<?php echo site_url("importer_airtel/importer"); ?>', // 
//             type: 'POST',
//             data: formData,
//             processData: false, 
//             contentType: false, 
//             success: function(data) {
                
//                 // console.log(data);
//                 // $('.spinner').hide();
//                 // var blob = new Blob([data], { type: 'data:application/vnd.ms-excel' });
//                 // var downloadUrl = URL.createObjectURL(blob);
//                 // var a = document.createElement("a");
//                 // a.href = downloadUrl;
//                 // a.download = "downloadFile.xlsx";
//                 // document.body.appendChild(a);
//                 // a.click();

//                 $('.spinner').hide();

//                 swal({
//                     title: "Succès",
//                     text: "Données importées avec succès",
//                     icon: "success",
//                     button: "OK",
//                 });    
//             },
//             error: function(error) {
//                 // Gérez les erreurs ici
//                 // hideSpinner(); 
//             }
//         });
//     });
// });
    
</script>